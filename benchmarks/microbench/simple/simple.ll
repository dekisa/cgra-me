; ModuleID = 'simple.bc'
source_filename = "simple.tagged.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@__const.main.a = private unnamed_addr constant [4 x i32] [i32 1, i32 2, i32 3, i32 4], align 16
@__const.main.b = private unnamed_addr constant [4 x i32] [i32 5, i32 6, i32 7, i32 8], align 16
@.str = private unnamed_addr constant [12 x i8] c"c[%d] = %d\0A\00", align 1

; Function Attrs: nounwind uwtable
define dso_local i32 @main() local_unnamed_addr #0 {
entry:
  %a = alloca [4 x i32], align 16
  %b = alloca [4 x i32], align 16
  %c = alloca [4 x i32], align 16
  %0 = bitcast [4 x i32]* %a to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* nonnull %0) #4
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull align 16 %0, i8* align 16 bitcast ([4 x i32]* @__const.main.a to i8*), i64 16, i1 true)
  %1 = bitcast [4 x i32]* %b to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* nonnull %1) #4
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull align 16 %1, i8* align 16 bitcast ([4 x i32]* @__const.main.b to i8*), i64 16, i1 true)
  %2 = bitcast [4 x i32]* %c to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* nonnull %2) #4
  call void @llvm.memset.p0i8.i64(i8* nonnull align 16 %2, i8 0, i64 16, i1 true)
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %indvars.iv31 = phi i64 [ 0, %entry ], [ %indvars.iv.next32, %for.body ]
  tail call void @DFGLOOP_TAG(i32 1) #4
  %arrayidx = getelementptr inbounds [4 x i32], [4 x i32]* %a, i64 0, i64 %indvars.iv31
  %3 = load volatile i32, i32* %arrayidx, align 4, !tbaa !2
  %arrayidx2 = getelementptr inbounds [4 x i32], [4 x i32]* %b, i64 0, i64 %indvars.iv31
  %4 = load volatile i32, i32* %arrayidx2, align 4, !tbaa !2
  %add = add nsw i32 %4, %3
  %arrayidx4 = getelementptr inbounds [4 x i32], [4 x i32]* %c, i64 0, i64 %indvars.iv31
  store volatile i32 %add, i32* %arrayidx4, align 4, !tbaa !2
  %indvars.iv.next32 = add nuw nsw i64 %indvars.iv31, 1
  %exitcond33 = icmp eq i64 %indvars.iv.next32, 4
  br i1 %exitcond33, label %for.body7, label %for.body

for.body7:                                        ; preds = %for.body, %for.body7
  %indvars.iv = phi i64 [ %indvars.iv.next, %for.body7 ], [ 0, %for.body ]
  %sum.029 = phi i32 [ %add12, %for.body7 ], [ 0, %for.body ]
  %arrayidx9 = getelementptr inbounds [4 x i32], [4 x i32]* %c, i64 0, i64 %indvars.iv
  %5 = load volatile i32, i32* %arrayidx9, align 4, !tbaa !2
  %6 = trunc i64 %indvars.iv to i32
  %call = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i64 0, i64 0), i32 %6, i32 %5)
  %7 = load volatile i32, i32* %arrayidx9, align 4, !tbaa !2
  %add12 = add nsw i32 %7, %sum.029
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond = icmp eq i64 %indvars.iv.next, 4
  br i1 %exitcond, label %for.end15, label %for.body7

for.end15:                                        ; preds = %for.body7
  call void @llvm.lifetime.end.p0i8(i64 16, i8* nonnull %2) #4
  call void @llvm.lifetime.end.p0i8(i64 16, i8* nonnull %1) #4
  call void @llvm.lifetime.end.p0i8(i64 16, i8* nonnull %0) #4
  ret i32 %add12
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1) #1

declare dso_local void @DFGLOOP_TAG(i32) local_unnamed_addr #2

; Function Attrs: nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #3

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #1

attributes #0 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (https://github.com/llvm/llvm-project.git 7f95f963911853cea81f01c919b46676d7a95f52)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"int", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
