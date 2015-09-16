-- expr.hs
{-# LANGUAGE ForeignFunctionInterface #-}
module Expr where
-- processing libraries
import Numeric (showHex, readHex)
import Text.ParserCombinators.Parsec
import Debug.Trace (trace)
import Control.Monad (liftM2)
import Data.Function (on)
import Data.List (sortBy)
import Data.Either (rights)
import Data.Word
-- exporting libraries
import Foreign
import Foreign.C.Types
import Foreign.C.String
import Foreign.Ptr

-- imports from C
foreign import ccall "expr_register_read" register_read_c :: CString -> IO Word32
foreign import ccall "expr_swaddr_read" swaddr_read_c :: Word32 -> IO Word32
-- exports to C
foreign export ccall "expr" expr_hs :: CString -> Ptr CBool -> IO Word32

-- expression structure definition
---- Memory unit type
type ValueType = Word32

---- Base Integer Type
data BaseInt = Dec ValueType | Hex ValueType | Reg String
validRegNames = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "eip"]
instance Show BaseInt where
    show (Dec x) = show x
    show (Hex x) = "0x" ++ showHex x ""
    show (Reg s) = "$" ++ s
getInt :: BaseInt -> IO ValueType
getInt x = case x of
    Dec n -> return n
    Hex n -> return n
    Reg s -> newCString s >>= register_read_c

---- Operator
data OpAssoc = AssocL | AssocR deriving (Eq)
data OpAry = Unary | Binary deriving (Eq, Show)
data Op = Op {
    getAssoc :: OpAssoc, 
    getAry :: OpAry, 
    getFunc :: ValueType -> ValueType -> IO ValueType,
    getOp :: String
}
instance Show Op where
    show op = getOp op

makeOp s = case lookup s $ map (\op -> (getOp op, op)) $ concat opDefs of
    Just op -> op
    Nothing -> error "Invalid operator"

liftOp f = \x y -> return $ f x y

opDefs' = [ -- operators with precedance
    [ Op AssocL Binary (liftOp opOr) "||" ],
    [ Op AssocL Binary (liftOp opAnd) "&&" ],
    [ Op AssocL Binary (liftOp opEq) "==", Op AssocL Binary (liftOp opNeq) "!=" ],
    [ Op AssocL Binary (liftOp (+)) "+", Op AssocL Binary (liftOp (-)) "-" ],
    [ Op AssocL Binary (liftOp (*)) "*", Op AssocL Binary (liftOp div) "/" ],
    [ Op AssocR Unary (liftOp opNot) "!"],
    [ Op AssocR Unary (liftOp (-)) "-", Op AssocR Unary (liftOp (+)) "+"],
    [ Op AssocR Unary opDeref "*"]
    ] where
        opEq i1 i2
            | i1 == i2 = 1
            | otherwise = 0
        opNeq i1 i2 = 1 - opEq i1 i2
        opAnd i1 i2
            | i1 /= 0 && i2 /= 0 = 1
            | otherwise = 0
        opOr i1 i2
            | i1 /= 0 || i2 /= 0 = 1
            | otherwise = 0
        opNot _ i
            | i == 0 = 1
            | otherwise = 0
        opDeref _ addr = do
            swaddr_read_c addr
opDefs = sortBy (compare `on` (negate . length . getOp)) `map` opDefs'

---- Expression
data Expr = ExprNone
    | Number BaseInt
    | Patherness Expr
    | UnOp Op Expr
    | BiOp Op Expr Expr

printExpr (Number x) = show x
printExpr (Patherness x) = "(" ++ printExpr x ++ ")"
printExpr (UnOp op x) = show op ++ printExpr x
printExpr (BiOp op x y) = printExpr x ++ " " ++ show op ++ " " ++ printExpr y

evalExpr (Number x) = getInt x
evalExpr (Patherness x) = evalExpr x
evalExpr (UnOp op x) = evalExpr x >>= getFunc op 0
evalExpr (BiOp op x y) = evalExpr x >>= \x' -> evalExpr y >>= getFunc op x'

parseExpr str = parse (exprP 0 >>= \exp -> spaces >> eof >> return exp) "" str

instance Show Expr where
    show exp = printExpr exp

-- Parser Combinators
---- Number
decP =  -- [0-9]+
    many1 digit >>= return . Number . Dec . read
hexP = do -- 0(x|X)[0-9a-fA-F]+
    char '0'
    char 'x' <|> char 'X'
    s <- many1 hexDigit
    let [(num, _)] = readHex s
    return $ Number $ Hex num
numP =  -- Num = Dec | Hex
    (lookAhead digit >> (try hexP <|> decP))

---- Expression in patherness
pathernessP = -- Patherness = (Expr)
    between    (char '(' >> spaces) ((spaces >> char ')') <|> fail "patherness does not match") (exprP 0) >>= return . Patherness

---- Register
registerP = do -- Register = $ <RegName>
    char '$'
    spaces
    name <- (try . choice . map string) validRegNames <|> fail "invalid register"
    return . Number . Reg $ name

unitP =  -- Unit = Patherness | Num
    numP <|> registerP <|> pathernessP <|> fail "require operand"

exprP :: Int -> CharParser () Expr
exprP precedance -- Expr[p] = Unary[p] | Binary[p]
    | precedance >= length opDefs = unitP
    | otherwise = try (unaryP precedance) <|> try (binaryP precedance)
unaryP precedance = do -- Unary[p] = <UnaryOp> Expr[p+1] Expr'[p] WARNING: Associativity is ignored now
    spaces
    op <- foldl (flip (<|>)) (fail "require unary operator") $ map (try . operatorP) $ filter ((==Unary) . getAry) $ opDefs !! precedance
    spaces
    exprP (precedance + 1) >>= exprP' precedance . UnOp op
binaryP precedance = spaces >> exprP (precedance + 1) >>= liftM2 (<|>) (exprP' precedance) return  -- Binary[p] = Expr[p+1] Expr'[p]
exprP' :: Int -> Expr -> CharParser () Expr
exprP' precedance exp1 = flip (<|>) (return exp1) $ try $ do -- Expr'[p] = <BiOp> Expr[p+1] Exor'[p] | e
    spaces
    op <- foldl (flip (<|>)) (fail "require binary operator") $ map (try . operatorP) $ filter ((==Binary) . getAry) $ opDefs !! precedance
    spaces
    exp2 <- exprP $ precedance + 1
    case getAssoc op of
        AssocL -> exprP' precedance $ BiOp op exp1 exp2
        AssocR -> exprP' precedance exp2 >>= return . BiOp op exp1 
operatorP :: Op -> CharParser () Op
operatorP op = do
    (string . getOp) op
    return op

-- Exporting implements here
---- Bool type
type CBool = Word8
---- uint32_t expr(char *, bool *)
expr_hs cstr pbool = do
    str <- peekCString cstr
    let exp = parseExpr str
    case exp of
        Right res -> do
            poke pbool 1
            evalExpr res
        Left err -> do
            poke pbool 0
            putStr "In expression:\n\t"
            putStrLn str
            let pos = sourceColumn $ errorPos err
            putStrLn $ "\t" ++ (replicate (pos - 1) ' ') ++ "^"
            print err
            return 0
