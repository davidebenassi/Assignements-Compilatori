; ModuleID = 'loop.ll'
source_filename = "loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

define dso_local void @foo(i32 noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3, ptr noundef %4) {
  br label %6

6:                                                ; preds = %18, %5
  %.02 = phi i32 [ 0, %5 ], [ %19, %18 ]
  %7 = icmp slt i32 %.02, %0
  br i1 %7, label %8, label %20

8:                                                ; preds = %6
  %9 = sext i32 %.02 to i64
  %10 = getelementptr inbounds i32, ptr %2, i64 %9
  %11 = load i32, ptr %10, align 4
  %12 = sext i32 %.02 to i64
  %13 = getelementptr inbounds i32, ptr %3, i64 %12
  %14 = load i32, ptr %13, align 4
  %15 = mul nsw i32 %11, %14
  %16 = sext i32 %.02 to i64
  %17 = getelementptr inbounds i32, ptr %1, i64 %16
  store i32 %15, ptr %17, align 4
  br label %18

18:                                               ; preds = %8
  %19 = add nsw i32 %.02, 1
  br label %6, !llvm.loop !4

20:                                               ; preds = %6
  br label %21

21:                                               ; preds = %33, %20
  %.01 = phi i32 [ 0, %20 ], [ %34, %33 ]
  %22 = icmp slt i32 %.01, %0
  br i1 %22, label %23, label %35

23:                                               ; preds = %21
  %24 = sext i32 %.01 to i64
  %25 = getelementptr inbounds i32, ptr %1, i64 %24
  %26 = load i32, ptr %25, align 4
  %27 = sext i32 %.01 to i64
  %28 = getelementptr inbounds i32, ptr %3, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %26, %29
  %31 = sext i32 %.01 to i64
  %32 = getelementptr inbounds i32, ptr %4, i64 %31
  store i32 %30, ptr %32, align 4
  br label %33

33:                                               ; preds = %23
  %34 = add nsw i32 %.01, 1
  br label %21, !llvm.loop !6

35:                                               ; preds = %21
  br label %36

36:                                               ; preds = %45, %35
  %.0 = phi i32 [ 0, %35 ], [ %46, %45 ]
  %37 = icmp slt i32 %.0, %0
  br i1 %37, label %38, label %47

38:                                               ; preds = %36
  %39 = sext i32 %.0 to i64
  %40 = getelementptr inbounds i32, ptr %3, i64 %39
  %41 = load i32, ptr %40, align 4
  %42 = sub nsw i32 %41, 1
  %43 = sext i32 %.0 to i64
  %44 = getelementptr inbounds i32, ptr %1, i64 %43
  store i32 %42, ptr %44, align 4
  br label %45

45:                                               ; preds = %38
  %46 = add nsw i32 %.0, 1
  br label %36, !llvm.loop !7

47:                                               ; preds = %36
  ret void
}

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 2}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 18.1.6 (Fedora 18.1.6-3.fc40)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !5}
!7 = distinct !{!7, !5}
