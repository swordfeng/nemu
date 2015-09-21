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
import Data.Functor (($>))
-- exporting libraries
import Foreign
import Foreign.C.Types
import Foreign.C.String
import Foreign.Ptr

---- C Bool type
type CBool = Word8
-- imports from C
foreign import ccall unsafe "expr_register_read" register_read_c :: CString -> IO Word32
foreign import ccall unsafe "expr_swaddr_read" swaddr_read_c :: Word32 -> IO Word32
-- exports to C
foreign export ccall "expr" expr_hs :: CString -> Ptr CBool -> IO Word32
foreign export ccall "expr_prettify" expr_prettify_hs :: CString -> Ptr CString -> IO CBool

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
    getFunc :: IO ValueType -> IO ValueType -> IO ValueType,
    getOp :: String
}
instance Show Op where
    show op = getOp op

makeOp s = case lookup s $ map (\op -> (getOp op, op)) $ concat opDefs of
    Just op -> op
    Nothing -> error "Invalid operator"

opDefs' = [ -- operators with precedance
    [ Op AssocL Binary (liftM2 opOr) "||" ],
    [ Op AssocL Binary (liftM2 opAnd) "&&" ],
    [ Op AssocL Binary (liftM2 opEq) "==", Op AssocL Binary (liftM2 opNeq) "!=" ],
    [ Op AssocL Binary (liftM2 (+)) "+", Op AssocL Binary (liftM2 (-)) "-" ],
    [ Op AssocL Binary (liftM2 (*)) "*", Op AssocL Binary (liftM2 div) "/" ],
    [ Op AssocR Unary (liftM2 opNot) "!"],
    [ Op AssocR Unary (liftM2 (-)) "-", Op AssocR Unary (liftM2 (+)) "+"],
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
            addr >>= swaddr_read_c
opDefs = sortBy (compare `on` (negate . length . getOp)) `map` opDefs'

---- Expression
data Expr = ExprNone
    | Number BaseInt
    | Patherness Expr
    | UnOp Op Expr
    | BiOp Op Expr Expr

showExpr (Number x) = show x
showExpr (Patherness x) = "(" ++ showExpr x ++ ")"
showExpr (UnOp op x) = show op ++ showExpr x
showExpr (BiOp op x y) = showExpr x ++ " " ++ show op ++ " " ++ showExpr y

evalExpr :: Expr -> IO ValueType
evalExpr (Number x) = getInt x
evalExpr (Patherness x) = evalExpr x
evalExpr (UnOp op x) = getFunc op (pure 0) (evalExpr x)
evalExpr (BiOp op x y) = getFunc op (evalExpr x) (evalExpr y)

parseExpr str = parse (exprP 0 >>= \exp -> empty >> eof $> exp) "" str

instance Show Expr where
    show exp = showExpr exp

-- Parser Combinators
---- Empty
empty = spaces <?> ""
---- Number
decP = -- [0-9]+
    Number . Dec . read <$> many1 digit
hexP = -- 0(x|X)[0-9a-fA-F]+
        Number . Hex . fst . head . readHex <$> (char '0' *> (char 'x' <|> char 'X') *> many1 hexDigit)
numP =  -- Num = Dec | Hex
    try hexP <|> decP <?> "digit" 
---- Register
registerP = -- Register = $ <RegName>
    fmap (Number . Reg) $ char '$' *> empty *> choice (map (try . string) validRegNames ++ [fail "invalid register name"])
---- Expression in patherness
pathernessP = -- Patherness = (Expr)
    Patherness `fmap` between (char '(' >> empty) (empty *> char ')' <|> fail "patherness does not match") (exprP 0)

---- Operators
operatorP :: Op -> CharParser () Op
operatorP op = 
    (string . getOp) op $> op
opClassP precedance aryType = 
    choice $ map (try . operatorP) $ filter ((== aryType) . getAry) $ opDefs !! precedance

operandP = -- Operand = Patherness | Num
    numP <|> registerP <|> pathernessP <|> fail "require operand"

exprP :: Int -> CharParser () Expr
exprP precedance -- Expr[p] = ExprL[p] ExprR[p] | Operand
    | precedance >= length opDefs = empty >> operandP
    | otherwise = empty >> exprLP precedance >>= exprRP precedance
exprLP precedance = flip (<|>) (exprP $ precedance + 1) $ try $ -- ExprL[p] = <UnaryOp> ExprL[p] | Expr[p+1]
    UnOp <$> (empty *> opClassP precedance Unary) <*> (empty *> exprLP precedance) <|> fail "require unary operator"
exprRP precedance exp1 = flip (<|>) (return exp1) $ try $ do -- ExprR[p] = <BiOp> ExprL[p] ExprR[p] | e
    op <- empty *> opClassP precedance Binary <|> fail "require binary operator"
    exp2 <- empty *> exprLP precedance
    case getAssoc op of
        AssocL -> exprRP precedance $ BiOp op exp1 exp2
        AssocR -> BiOp op exp1 <$> exprRP precedance exp2 

printErrInfo :: ParseError -> String -> IO ()
printErrInfo err str = do
    putStr "In expression:\n\t"
    putStrLn str
    let pos = sourceColumn $ errorPos err
    putStrLn $ "\t" ++ (replicate (pos - 1) ' ') ++ "^"
    print err

-- Exports are implemented here
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
            printErrInfo err str
            return 0
---- bool expr_prettify(char *, char **)
---- The second argument (pointer to result string) must be manually freed!
expr_prettify_hs cstr pstring = do
    str <- peekCString cstr
    let exp = parseExpr str
    case exp of
        Right res -> do
            newCString (show res) >>= poke pstring 
            return 1
        Left err -> do
            printErrInfo err str
            return 0
