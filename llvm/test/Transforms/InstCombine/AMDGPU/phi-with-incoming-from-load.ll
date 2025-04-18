; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 5
; RUN: opt -passes=instcombine -S -o - %s | FileCheck %s

target triple = "amdgcn-amd-amdhsa"

%double_double = type { double, double }

declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1)

define void @_test(ptr addrspace(4) byref(%double_double) align 8 %in)  {
; CHECK-LABEL: define void @_test(
; CHECK-SAME: ptr addrspace(4) byref([[DOUBLE_DOUBLE:%.*]]) align 8 [[IN:%.*]]) {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    [[ALPHA_UNION:%.*]] = addrspacecast ptr addrspace(4) [[IN]] to ptr
; CHECK-NEXT:    [[LOAD:%.*]] = load i8, ptr addrspace(5) null, align 1
; CHECK-NEXT:    [[LOADEDV:%.*]] = trunc i8 [[LOAD]] to i1
; CHECK-NEXT:    br i1 [[LOADEDV]], label %[[COND_END:.*]], label %[[COND_FALSE:.*]]
; CHECK:       [[COND_FALSE]]:
; CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr addrspace(4) [[IN]], align 8
; CHECK-NEXT:    br label %[[COND_END]]
; CHECK:       [[COND_END]]:
; CHECK-NEXT:    [[COND1:%.*]] = phi ptr [ [[TMP0]], %[[COND_FALSE]] ], [ [[ALPHA_UNION]], %[[ENTRY]] ]
; CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 1 dereferenceable(16) poison, ptr noundef nonnull align 1 dereferenceable(16) [[COND1]], i64 16, i1 false)
; CHECK-NEXT:    ret void
;
entry:
  %coerce = alloca %double_double, align 8, addrspace(5)
  %alpha_union = addrspacecast ptr addrspace(5) %coerce to ptr
  call void @llvm.memcpy.p5.p4.i64(ptr addrspace(5) align 8 %coerce, ptr addrspace(4) align 8 %in, i64 16, i1 false)
  %load1 = load i8, ptr addrspace(5) null, align 1
  %loadedv = trunc i8 %load1 to i1
  br i1 %loadedv, label %cond.end, label %cond.false

cond.false:
  %load2 = load ptr, ptr addrspace(5) %coerce, align 8
  br label %cond.end

cond.end:
  %cond = phi ptr [ %load2, %cond.false ], [ %alpha_union, %entry ]
  call void @llvm.memcpy.p0.p0.i64(ptr poison, ptr %cond, i64 16, i1 false)
  ret void
}
