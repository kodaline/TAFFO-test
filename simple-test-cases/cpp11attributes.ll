; ModuleID = 'cpp11attributes.ll.4.magiclangtmp.ll'
source_filename = "cpp11attributes.cpp"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@.str = private unnamed_addr constant [17 x i8] c"range -3000 3000\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [20 x i8] c"cpp11attributes.cpp\00", section "llvm.metadata"

; Function Attrs: noinline nounwind ssp uwtable
define float @_Z4oveniif(i32 %stuff, i32 %baked, float %cherry) #0 !taffo.funinfo !3 {
entry:
  %stuff.addr = alloca i32, align 4
  %baked.addr = alloca i32, align 4
  %cherry.addr = alloca float, align 4
  %cake.s13_19fixp = alloca i32, align 4, !taffo.info !5
  store i32 %stuff, i32* %stuff.addr, align 4
  store i32 %baked, i32* %baked.addr, align 4
  store float %cherry, float* %cherry.addr, align 4
  %0 = load i32, i32* %baked.addr, align 4
  %1 = load i32, i32* %stuff.addr, align 4
  %add = add nsw i32 %0, %1, !taffo.info !8
  %2 = shl i32 %add, 19, !taffo.info !8
  store i32 %2, i32* %cake.s13_19fixp, align 4, !taffo.info !8
  %s13_19fixp = load i32, i32* %cake.s13_19fixp, align 4, !taffo.info !8
  %3 = load float, float* %cherry.addr, align 4
  %4 = fmul float 5.242880e+05, %3, !taffo.info !8
  %5 = fptosi float %4 to i32, !taffo.info !8
  %add2.s13_19fixp = add i32 %s13_19fixp, %5, !taffo.info !8
  %6 = sitofp i32 %add2.s13_19fixp to float, !taffo.info !8
  %7 = fdiv float %6, 5.242880e+05, !taffo.info !8
  ret float %7, !taffo.info !8
}

; Function Attrs: nounwind
declare !taffo.funinfo !10 void @llvm.var.annotation(i8*, i8*, i8*, i32) #1

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 6.0.1 (tags/RELEASE_601/final)"}
!3 = !{i32 1, !4, i32 1, !4, i32 1, !4}
!4 = !{i1 false, i1 false, i1 false}
!5 = !{!6, !7, i1 false}
!6 = !{!"fixp", i32 -32, i32 19}
!7 = !{double -3.000000e+03, double 3.000000e+03}
!8 = !{!6, !9, i1 false}
!9 = !{double 0x7FF8000000000000, double 0x7FF8000000000000}
!10 = !{i32 1, !4, i32 1, !4, i32 1, !4, i32 1, !4}
