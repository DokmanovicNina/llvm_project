; ModuleID = '1.c'
source_filename = "1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [34 x i8] c"Student je polozio, povrsina: %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i16, align 2
  %6 = alloca i16, align 2
  %7 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %8 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %2)
  store i32 100, ptr %3, align 4
  store i32 200, ptr %4, align 4
  %9 = load i32, ptr %3, align 4
  %10 = trunc i32 %9 to i16
  store i16 %10, ptr %5, align 2
  %11 = load i32, ptr %4, align 4
  %12 = trunc i32 %11 to i16
  store i16 %12, ptr %6, align 2
  %13 = load i32, ptr %2, align 4
  %14 = icmp eq i32 %13, 6
  br i1 %14, label %21, label %15

15:                                               ; preds = %0
  %16 = load i32, ptr %2, align 4
  %17 = icmp eq i32 %16, 7
  br i1 %17, label %21, label %18

18:                                               ; preds = %15
  %19 = load i32, ptr %2, align 4
  %20 = icmp eq i32 %19, 8
  br i1 %20, label %21, label %29

21:                                               ; preds = %18, %15, %0
  %22 = load i16, ptr %5, align 2
  %23 = sext i16 %22 to i32
  %24 = load i16, ptr %6, align 2
  %25 = sext i16 %24 to i32
  %26 = add nsw i32 %23, %25
  store i32 %26, ptr %7, align 4
  %27 = load i32, ptr %7, align 4
  %28 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %27)
  br label %29

29:                                               ; preds = %21, %18
  ret i32 0
}

declare i32 @__isoc99_scanf(ptr noundef, ...) #1

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 17.0.0"}
