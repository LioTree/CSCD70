; RUN: opt -S -load-pass-plugin=%dylibdir/libDFA.so \
; RUN:     -p=liveness %s -o %basename_t 2>%basename_t.log
; @done(CSCD70): FileCheck --match-full-lines %s --input-file=%basename_t.log

; int sum(int a, int b) {
;   int res = 1;
;   for (int i = a; i < b; i++) {
;     res += i;
;   }
;   return res;
; }
; @done(CSCD70) Please complete the CHECK directives.
define i32 @sum(i32 noundef %0, i32 noundef %1) {
  br label %3
; CHECK: [Liveness]      {i32 %0, i32 %1, }
; CHECK-EMPTY:
; CHECK: [Liveness]      {i32 %0, i32 %1, }

3:                                                ; preds = %7, %2
  %.01 = phi i32 [ 1, %2 ], [ %6, %7 ]
; CHECK: [Liveness]      {i32 %6, i32 %0, i32 %8, i32 %1, }
  %.0 = phi i32 [ %0, %2 ], [ %8, %7 ]
; CHECK: [Liveness]      {i32 %0, i32 %8, i32 %1, i32 %.01, }
  %4 = icmp slt i32 %.0, %1
; CHECK: [Liveness]      {i32 %.0, i32 %1, i32 %.01, }
  br i1 %4, label %5, label %9
; CHECK: [Liveness]      {i32 %.0, i32 %1, i1 %4, i32 %.01, }
; CHECK-EMPTY:
; CHECK: [Liveness]      {i32 %.0, i32 %1, i32 %.01, }

5:                                                ; preds = %3
  %6 = add nsw i32 %.01, %.0
; CHECK: [Liveness]      {i32 %.0, i32 %1, i32 %.01, }
  br label %7
; CHECK: [Liveness]      {i32 %6, i32 %.0, i32 %1, }
; CHECK-EMPTY:
; CHECK: [Liveness]      {i32 %6, i32 %.0, i32 %1, }

7:                                                ; preds = %5
  %8 = add nsw i32 %.0, 1
; CHECK: [Liveness]      {i32 %6, i32 %.0, i32 %1, }
  br label %3
; CHECK: [Liveness]      {i32 %6, i32 %8, i32 %1, }
; CHECK-EMPTY:
; CHECK: [Liveness]      {i32 %6, i32 %8, i32 %1, }

9:                                                ; preds = %3
  ret i32 %.01
; CHECK: [Liveness]      {i32 %.01, }
; CHECK-EMPTY:
; CHECK: [Liveness]      {}
}