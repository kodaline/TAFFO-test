	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 14
	.section	__TEXT,__literal4,4byte_literals
	.p2align	2               ## -- Begin function test
LCPI0_0:
	.long	1308622848              ## float 536870912
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_test
	.p2align	4, 0x90
_test:                                  ## @test
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movss	%xmm0, -4(%rbp)
	movl	%edi, -8(%rbp)
	movl	$1073741824, -16(%rbp)  ## imm = 0x40000000
	movss	-4(%rbp), %xmm0         ## xmm0 = mem[0],zero,zero,zero
	movss	LCPI0_0(%rip), %xmm1    ## xmm1 = mem[0],zero,zero,zero
	mulss	%xmm1, %xmm0
	cvttss2si	%xmm0, %rax
	movl	%eax, %edi
	movl	-16(%rbp), %ecx
	movl	%ecx, %eax
	movl	%edi, %ecx
	movl	%ecx, %edx
	imulq	%rdx, %rax
	shrq	$29, %rax
	movl	%eax, %ecx
	movl	%ecx, -16(%rbp)
	movl	-8(%rbp), %ecx
	shll	$29, %ecx
	addl	-16(%rbp), %ecx
	movl	%ecx, -16(%rbp)
	movl	-16(%rbp), %ecx
	shrl	$29, %ecx
	movl	%ecx, -12(%rbp)
	movl	-12(%rbp), %ecx
	cvtsi2ssl	%ecx, %xmm0
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3               ## -- Begin function test2
LCPI1_0:
	.quad	4611686018427387904     ## double 2
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_test2
	.p2align	4, 0x90
_test2:                                 ## @test2
	.cfi_startproc
## %bb.0:                               ## %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movsd	LCPI1_0(%rip), %xmm0    ## xmm0 = mem[0],zero
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %edi
	cvtsi2sdl	%edi, %xmm1
	addsd	%xmm0, %xmm1
	cvttsd2si	%xmm1, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.comm	_global,4,2             ## @global

.subsections_via_symbols
