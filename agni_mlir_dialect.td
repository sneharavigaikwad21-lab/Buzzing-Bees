// MLIR TableGen definition file

include "mlir/IR/OpBase.td"
include "mlir/IR/BuiltinTypes.td"

def Agni_Dialect : Dialect {
    let name = "agni";
    let description = [{
        AGNI Hardware Dialect for Project AGNI
        Targets: Wrench (Gemmini RoCC), Key (RVV 1.0)
        Source: TRM v3.0, Real Hardware Specs
    }];
    let cppNamespace = "::mlir::agni";
}

// ============================================================================
// WRENCH (Systolic Array) OPERATIONS
// ============================================================================

def AgniWrenchConfigOp : Op<Agni_Dialect, "wrench.config"> {
    let summary = "Configure Wrench systolic array dimensions";
    let arguments = (ins I32Attr:$rows, I32Attr:$cols);
    let assemblyFormat = "attr-dict";
}

def AgniWrenchLoadAOp : Op<Agni_Dialect, "wrench.load_a"> {
    let summary = "Load matrix A into Wrench L2";
    let arguments = (ins I64:$addr);
    let assemblyFormat = "$addr attr-dict";
}

def AgniWrenchLoadBOp : Op<Agni_Dialect, "wrench.load_b"> {
    let summary = "Load matrix B into Wrench L2";
    let arguments = (ins I64:$addr);
    let assemblyFormat = "$addr attr-dict";
}

def AgniWrenchExecuteOp : Op<Agni_Dialect, "wrench.execute"> {
    let summary = "Execute Wrench MAC operation";
    let arguments = (ins I64:$output_addr);
    let assemblyFormat = "$output_addr attr-dict";
}

// ============================================================================
// KEY (RVV 1.0) OPERATIONS
// ============================================================================

def AgniKeyVectorOp : Op<Agni_Dialect, "key.vector"> {
    let summary = "RVV 1.0 vector operation";
    let arguments = (ins StrAttr:$operation, I64:$vtype);
    let assemblyFormat = "$operation attr-dict";
}

// ============================================================================
// NOC (Network-on-Chip) OPERATIONS
// ============================================================================

def AgniNocCopyAsyncOp : Op<Agni_Dialect, "noc.copy_async"> {
    let summary = "Async NOC copy (fire-and-forget)";
    let arguments = (ins I64:$src, I64:$dst, I32:$size);
    let assemblyFormat = "$src \",\" $dst \",\" $size attr-dict";
}

def AgniNocWaitOp : Op<Agni_Dialect, "noc.wait"> {
    let summary = "Wait for NOC transfer completion";
    let arguments = (ins I32:$timeout);
    let assemblyFormat = "$timeout attr-dict";
}

// ============================================================================
// MEMORY OPERATIONS
// ============================================================================

def AgniMemcpyL2Op : Op<Agni_Dialect, "memcpy.l2"> {
    let summary = "Copy between L2 SRAM regions";
    let arguments = (ins I64:$dst, I64:$src, I32:$size);
    let assemblyFormat = "$dst \",\" $src \",\" $size attr-dict";
}