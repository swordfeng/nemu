-- expr.hs
{-# LANGUAGE ForeignFunctionInterface, CPP #-}
#include "common.h"
module Expr where
#ifndef DEEP_PERFORMANCE
-- processing libraries
import Numeric (showHex, readHex)
import Text.Parsec
import Text.Parsec.String (Parser)
import Debug.Trace (trace)
import Control.Monad (liftM2)
import Data.Function (on)
import Data.List (sortBy)
import Data.Either (rights)
import Data.Word
import Data.Functor (($>))
import Data.Bits ((.&.), (.|.))
-- exporting libraries
import Foreign
import Foreign.C.Types
import Foreign.C.String
import Foreign.Ptr
import System.IO.Unsafe (unsafePerformIO)

---- C Bool type
type CBool = Word8
---- C ExprFun type
type CExprFun = Ptr CBool -> IO Word32
-- imports from C
foreign import ccall unsafe "reg_name_mask" reg_name_mask :: CString -> IO Word32
foreign import ccall unsafe "reg_name_ptr" reg_name_ptr :: CString -> IO (Ptr Word32)
foreign import ccall unsafe "lnaddr_read" lnaddr_read :: Word32 -> CSize -> IO Word32
foreign import ccall unsafe "elf_find_sym" elf_find_sym_c :: CString -> Ptr Word32 -> IO CBool
-- imports from Haskell library
foreign import ccall unsafe "wrapper" makeCExprFun :: CExprFun -> IO (FunPtr CExprFun)
-- exports to C
foreign export ccall "expr" expr_hs :: CString -> Ptr CBool -> IO Word32
foreign export ccall "expr_prettify" expr_prettify_hs :: CString -> Ptr CString -> IO CBool
foreign export ccall "new_expr_fun" new_expr_fun_hs :: CString -> IO (FunPtr CExprFun)
foreign export ccall "free_expr_fun" free_expr_fun_hs :: FunPtr CExprFun -> IO ()

-- ELF Symbol finding helper
elf_find_sym :: String -> Maybe Word32
elf_find_sym name = unsafePerformIO . alloca $ \pValue -> do
  succ <- withCString name $ flip elf_find_sym_c pValue
  if succ /= 0 then
    peek pValue >>= return . Just
  else
    return Nothing

-- expression structure definition
---- Memory unit type
type ValueType = Word32

-- ShowExpr
-- Types can be showed as Expr
class ShowExpr a where
  showExpr :: a -> String

---- Base Integer Type
data BaseInt = Dec ValueType | Hex ValueType | Reg String Word32 (Ptr Word32) | Symbol String Word32
validRegNames = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "eflags", "eip",
    "cr0", "cr3", "es", "cs", "ss" , "ds", "fs", "gs"]
instance ShowExpr BaseInt where
    showExpr (Dec x) = show x
    showExpr (Hex x) = "0x" ++ showHex x ""
    showExpr (Reg s m p) = "$" ++ s
    showExpr (Symbol name val) = name
getInt :: BaseInt -> IO ValueType
getInt x = case x of
    Dec n -> return n
    Hex n -> return n
    Reg s m p -> do
      val <- peek p
      return $ val .&. m
    Symbol name val -> return val

---- Reg creater
---- Actually Reference Transparent
makeReg :: String -> BaseInt
makeReg name = unsafePerformIO $ do
    mask <- withCString name reg_name_mask
    ptr <- withCString name reg_name_ptr
    return $ Reg name mask ptr

---- Operator
data OpAssoc = AssocL | AssocR deriving (Eq)
data OpAry = Unary | Binary deriving (Eq)
data Op = Op {
    getAssoc :: OpAssoc,
    getAry :: OpAry,
    getFunc :: IO ValueType -> IO ValueType -> IO ValueType,
    getOp :: String
}
instance ShowExpr Op where
    showExpr op = getOp op

makeOp s = case lookup s $ map (\op -> (getOp op, op)) $ concat opDefs of
    Just op -> op
    Nothing -> error "Invalid operator"

opDefs' = [ -- operators with precedance
    [ Op AssocL Binary (liftM2 opOr) "||" ],
    [ Op AssocL Binary (liftM2 opAnd) "&&" ],
    [ Op AssocL Binary (liftM2 opEq) "==", Op AssocL Binary (liftM2 opNeq) "!=", Op AssocL Binary (liftM2 opLT) "<", Op AssocL Binary (liftM2 opGT) ">", Op AssocL Binary (liftM2 opLTE) "<=", Op AssocL Binary (liftM2 opGTE) ">=" ],
    [ Op AssocL Binary (liftM2 (+)) "+", Op AssocL Binary (liftM2 (-)) "-" ],
    [ Op AssocL Binary (liftM2 (*)) "*", Op AssocL Binary (liftM2 div) "/", Op AssocL Binary (liftM2 mod) "%" ],
    [ Op AssocR Unary (liftM2 opNot) "!"],
    [ Op AssocR Unary (liftM2 (-)) "-", Op AssocR Unary (liftM2 (+)) "+"],
    [ Op AssocR Unary opDeref "*"]
    ] where
        bool2int True = 1
        bool2int False = 0
        opEq i1 i2 = bool2int $ i1 == i2
        opNeq i1 i2 = bool2int $ i1 /= i2
        opAnd i1 i2 = bool2int $ i1 /= 0 && i2 /= 0
        opOr i1 i2 = bool2int $ i1 /= 0 || i2 /= 0
        opNot _ i = bool2int $ i == 0
        opDeref _ addr = addr >>= flip lnaddr_read 4
        opLT i1 i2 = bool2int $ i1 < i2
        opGT i1 i2 = bool2int $ i1 > i2
        opLTE i1 i2 = bool2int $ i1 <= i2
        opGTE i1 i2 = bool2int $ i1 >= i2
opDefs = sortBy (compare `on` (negate . length . getOp)) `map` opDefs'

---- Expression
data Expr = ExprNone
    | Number BaseInt
    | Patherness Expr
    | UnOp Op Expr
    | BiOp Op Expr Expr

instance ShowExpr Expr where
    showExpr (Number x) = showExpr x
    showExpr (Patherness x) = "(" ++ showExpr x ++ ")"
    showExpr (UnOp op x) = showExpr op ++ showExpr x
    showExpr (BiOp op x y) = showExpr x ++ " " ++ showExpr op ++ " " ++ showExpr y

evalExpr :: Expr -> IO ValueType
evalExpr (Number x) = getInt x
evalExpr (Patherness x) = evalExpr x
evalExpr (UnOp op x) = getFunc op (pure 0) (evalExpr x)
evalExpr (BiOp op x y) = getFunc op (evalExpr x) (evalExpr y)

parseExpr str = parse (exprP 0 >>= \exp -> empty >> eof $> exp) "" str

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
    fmap (Number . makeReg) $ char '$' *> empty *> choice (map (try . string) validRegNames ++ [fail "invalid register name"])
---- Symbol
symbolP = do -- [a-zA-Z_][a-zA-Z0-9_]
    name <- (:) <$> (t <?> "symbol") <*> many (t <|> digit <?> "")
    case elf_find_sym name of
      Nothing -> fail $ "symbol \"" ++ name ++ "\" does not exist"
      Just value -> return . Number $ Symbol name value
    where
      t :: Parser Char
      t = upper <|> lower <|> char '_'
---- Expression in patherness
pathernessP = -- Patherness = (Expr)
    Patherness `fmap` between (char '(' >> empty) (empty *> char ')' <|> fail "patherness does not match") (exprP 0)

---- Operators
operatorP :: Op -> Parser Op
operatorP op =
    (string . getOp) op $> op
opClassP precedance aryType =
    choice $ map (try . operatorP) $ filter ((== aryType) . getAry) $ opDefs !! precedance

operandP = -- Operand = Num | Register | Symbol | Patherness
    numP <|> registerP <|> symbolP <|> pathernessP <|> fail "require operand"

exprP :: Int -> Parser Expr
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
            newCString (showExpr res) >>= poke pstring
            return 1
        Left err -> do
            printErrInfo err str
            return 0

makeExprFun :: Expr -> CExprFun
makeExprFun exp = \pbool -> poke pbool 1 >> evalExpr exp

new_expr_fun_hs cstr = do
  str <- peekCString cstr
  let exp = parseExpr str
  case exp of
      Left err -> do
          printErrInfo err str
          return nullFunPtr
      Right res -> do
          makeCExprFun . makeExprFun $ res

free_expr_fun_hs = freeHaskellFunPtr
#else
nullexp = ()
#endif
