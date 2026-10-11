	.file	"bench.cpp"
	.text
	.section	.text._ZNSt6thread24_M_thread_deps_never_runEv,"axG",@progbits,_ZNSt6thread24_M_thread_deps_never_runEv,comdat
	.p2align 4
	.weak	_ZNSt6thread24_M_thread_deps_never_runEv
	.type	_ZNSt6thread24_M_thread_deps_never_runEv, @function
_ZNSt6thread24_M_thread_deps_never_runEv:
.LFB7296:
	.cfi_startproc
	ret
	.cfi_endproc
.LFE7296:
	.size	_ZNSt6thread24_M_thread_deps_never_runEv, .-_ZNSt6thread24_M_thread_deps_never_runEv
	.text
	.p2align 4
	.type	_ZN12_GLOBAL__N_1L12plain_kernelERmm, @function
_ZN12_GLOBAL__N_1L12plain_kernelERmm:
.LFB7496:
	.cfi_startproc
	testq	%rsi, %rsi
	je	.L3
	xorl	%eax, %eax
	.p2align 4
	.p2align 3
.L5:
	addq	$1, (%rdi)
	addq	$1, %rax
	cmpq	%rax, %rsi
	jne	.L5
.L3:
	ret
	.cfi_endproc
.LFE7496:
	.size	_ZN12_GLOBAL__N_1L12plain_kernelERmm, .-_ZN12_GLOBAL__N_1L12plain_kernelERmm
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8856:
	.cfi_startproc
	testl	%edx, %edx
	je	.L11
	cmpl	$1, %edx
	je	.L12
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L12:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L11:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8856:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8863:
	.cfi_startproc
	testl	%edx, %edx
	je	.L15
	cmpl	$1, %edx
	je	.L16
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L16:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L15:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8863:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8870:
	.cfi_startproc
	testl	%edx, %edx
	je	.L19
	cmpl	$1, %edx
	je	.L20
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L20:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L19:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8870:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8877:
	.cfi_startproc
	testl	%edx, %edx
	je	.L23
	cmpl	$1, %edx
	je	.L24
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L24:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L23:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8877:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8884:
	.cfi_startproc
	testl	%edx, %edx
	je	.L27
	cmpl	$1, %edx
	je	.L28
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L28:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L27:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8884:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8891:
	.cfi_startproc
	testl	%edx, %edx
	je	.L31
	cmpl	$1, %edx
	je	.L32
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L32:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L31:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8891:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8898:
	.cfi_startproc
	testl	%edx, %edx
	je	.L35
	cmpl	$1, %edx
	je	.L36
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L36:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L35:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8898:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8905:
	.cfi_startproc
	testl	%edx, %edx
	je	.L39
	cmpl	$1, %edx
	je	.L40
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L40:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L39:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8905:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8912:
	.cfi_startproc
	testl	%edx, %edx
	je	.L43
	cmpl	$1, %edx
	je	.L44
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L44:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L43:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8912:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8919:
	.cfi_startproc
	testl	%edx, %edx
	je	.L47
	cmpl	$1, %edx
	je	.L48
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L48:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L47:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8919:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8926:
	.cfi_startproc
	testl	%edx, %edx
	je	.L51
	cmpl	$1, %edx
	je	.L52
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L52:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L51:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8926:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8933:
	.cfi_startproc
	testl	%edx, %edx
	je	.L55
	cmpl	$1, %edx
	je	.L56
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L56:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L55:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8933:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation:
.LFB8940:
	.cfi_startproc
	testl	%edx, %edx
	je	.L59
	cmpl	$1, %edx
	je	.L60
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L60:
	movq	%rsi, (%rdi)
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L59:
	movq	$_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8940:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB9276:
	.cfi_startproc
	movq	8(%rdi), %rdx
	movq	%rsi, %rax
	movq	(%rdx), %rsi
	movslq	(%rax), %rdx
	movq	(%rdi), %rax
	leaq	(%rax,%rdx,8), %rdi
	jmp	_ZN12_GLOBAL__N_1L12plain_kernelERmm
	.cfi_endproc
.LFE9276:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB9284:
	.cfi_startproc
	movq	8(%rdi), %rcx
	movq	%rsi, %rdx
	movq	%rdi, %rax
	movslq	(%rdx), %rdi
	movq	(%rcx), %rsi
	salq	$6, %rdi
	addq	(%rax), %rdi
	jmp	_ZN12_GLOBAL__N_1L12plain_kernelERmm
	.cfi_endproc
.LFE9284:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB9294:
	.cfi_startproc
	movq	8(%rdi), %rcx
	movq	%rsi, %rdx
	movq	%rdi, %rax
	movslq	(%rdx), %rdi
	movq	(%rcx), %rsi
	salq	$7, %rdi
	addq	(%rax), %rdi
	jmp	_ZN12_GLOBAL__N_1L12plain_kernelERmm
	.cfi_endproc
.LFE9294:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm, @function
_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm:
.LFB7495:
	.cfi_startproc
	testq	%rsi, %rsi
	je	.L65
	xorl	%eax, %eax
	testb	$1, %sil
	je	.L67
	lock addq	$1, (%rdi)
	movl	$1, %eax
	cmpq	$1, %rsi
	je	.L77
	.p2align 4
	.p2align 4
	.p2align 3
.L67:
	lock addq	$1, (%rdi)
	lock addq	$1, (%rdi)
	addq	$2, %rax
	cmpq	%rax, %rsi
	jne	.L67
.L65:
	ret
.L77:
	ret
	.cfi_endproc
.LFE7495:
	.size	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm, .-_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB9247:
	.cfi_startproc
	movq	8(%rdi), %rdx
	movq	%rsi, %rax
	movq	(%rdx), %rsi
	movslq	(%rax), %rdx
	movq	(%rdi), %rax
	leaq	(%rax,%rdx,8), %rdi
	jmp	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	.cfi_endproc
.LFE9247:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB9255:
	.cfi_startproc
	movq	8(%rdi), %rcx
	movq	%rsi, %rdx
	movq	%rdi, %rax
	movslq	(%rdx), %rdi
	movq	(%rcx), %rsi
	salq	$6, %rdi
	addq	(%rax), %rdi
	jmp	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	.cfi_endproc
.LFE9255:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB9265:
	.cfi_startproc
	movq	8(%rdi), %rcx
	movq	%rsi, %rdx
	movq	%rdi, %rax
	movslq	(%rdx), %rdi
	movq	(%rcx), %rsi
	salq	$7, %rdi
	addq	(%rax), %rdi
	jmp	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	.cfi_endproc
.LFE9265:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB8776:
	.cfi_startproc
	movq	(%rdi), %rax
	movl	$_ZN12_GLOBAL__N_1L8a_sharedE, %edi
	movq	(%rax), %rsi
	jmp	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	.cfi_endproc
.LFE8776:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 2
	.p2align 4
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED2Ev, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED2Ev:
.LFB10176:
	.cfi_startproc
	movq	$_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE+16, (%rdi)
	jmp	_ZNSt6thread6_StateD2Ev
	.cfi_endproc
.LFE10176:
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED2Ev, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED2Ev
	.set	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED1Ev,_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED2Ev
	.align 2
	.p2align 4
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED0Ev, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED0Ev:
.LFB10178:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	$_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE+16, (%rdi)
	movq	%rdi, 8(%rsp)
	call	_ZNSt6thread6_StateD2Ev
	movq	8(%rsp), %rdi
	movl	$48, %esi
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	jmp	_ZdlPvm
	.cfi_endproc
.LFE10178:
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED0Ev, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED0Ev
	.section	.text.unlikely,"ax",@progbits
	.align 2
.LCOLDB0:
	.text
.LHOTB0:
	.align 2
	.p2align 4
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv:
.LFB10179:
	.cfi_startproc
	subq	$312, %rsp
	.cfi_def_cfa_offset 320
	movq	16(%rdi), %rax
	movq	%r12, 280(%rsp)
	.cfi_offset 12, -40
	movq	%rdi, %r12
	cmpb	$0, (%rax)
	jne	.L102
.L86:
	movq	24(%r12), %rax
	lock addl	$1, (%rax)
	.p2align 4
	.p2align 4
	.p2align 3
.L90:
	movq	32(%r12), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L90
	movl	8(%r12), %edx
	movq	40(%r12), %rax
	movl	%edx, 128(%rsp)
	cmpq	$0, 16(%rax)
	je	.L98
	leaq	128(%rsp), %rsi
	movq	%rax, %rdi
	call	*24(%rax)
	call	_ZNSt6chrono3_V212steady_clock3nowEv
	movq	%rax, %rdx
	movslq	8(%r12), %rax
	movq	280(%rsp), %r12
	salq	$8, %rax
	movq	%rdx, _ZN12_GLOBAL__N_1L4endsE(%rax)
	addq	$312, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L102:
	.cfi_restore_state
	pxor	%xmm0, %xmm0
	movq	%r14, 296(%rsp)
	movq	%rsp, %rdx
	.cfi_offset 14, -24
	movl	8(%rdi), %r14d
	movl	$128, %esi
	xorl	%edi, %edi
	movaps	%xmm0, (%rsp)
	movq	%r13, 288(%rsp)
	movaps	%xmm0, 16(%rsp)
	movaps	%xmm0, 32(%rsp)
	movaps	%xmm0, 48(%rsp)
	movaps	%xmm0, 64(%rsp)
	movaps	%xmm0, 80(%rsp)
	movaps	%xmm0, 96(%rsp)
	movaps	%xmm0, 112(%rsp)
	.cfi_offset 13, -32
	call	sched_getaffinity
	movl	%eax, %r13d
	testl	%eax, %eax
	jne	.L100
	movq	%rbx, 264(%rsp)
	.cfi_offset 3, -56
	xorl	%ebx, %ebx
	movq	%rbp, 272(%rsp)
	movq	%r15, 304(%rsp)
	.cfi_offset 6, -48
	.cfi_offset 15, -16
	jmp	.L89
	.p2align 4,,10
	.p2align 3
.L103:
	addl	$1, %r13d
.L87:
	addq	$1, %rbx
	cmpq	$1024, %rbx
	je	.L101
.L89:
	movq	%rbx, %rbp
	movl	%ebx, %r15d
	shrq	$6, %rbp
	andl	$63, %r15d
	movq	(%rsp,%rbp,8), %rax
	btq	%rbx, %rax
	jnc	.L87
	movq	%rsp, %rsi
	movl	$128, %edi
	call	__sched_cpucount
	movl	%eax, %esi
	movl	%r14d, %eax
	cltd
	idivl	%esi
	cmpl	%r13d, %edx
	jne	.L103
	pxor	%xmm0, %xmm0
	leaq	128(%rsp), %rbx
	movl	%r15d, %ecx
	movl	$1, %eax
	movaps	%xmm0, 128(%rsp)
	salq	%cl, %rax
	movaps	%xmm0, 144(%rsp)
	movaps	%xmm0, 160(%rsp)
	movaps	%xmm0, 176(%rsp)
	movaps	%xmm0, 192(%rsp)
	movaps	%xmm0, 208(%rsp)
	movaps	%xmm0, 224(%rsp)
	movaps	%xmm0, 240(%rsp)
	orq	%rax, (%rbx,%rbp,8)
	call	pthread_self
	movq	%rbx, %rdx
	movl	$128, %esi
	movq	%rax, %rdi
	call	pthread_setaffinity_np
.L101:
	movq	264(%rsp), %rbx
	.cfi_restore 3
	movq	272(%rsp), %rbp
	.cfi_restore 6
	movq	288(%rsp), %r13
	.cfi_restore 13
	movq	296(%rsp), %r14
	.cfi_restore 14
	movq	304(%rsp), %r15
	.cfi_restore 15
	jmp	.L86
	.p2align 4,,10
	.p2align 3
.L100:
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	movq	288(%rsp), %r13
	.cfi_restore 13
	movq	296(%rsp), %r14
	.cfi_restore 14
	jmp	.L86
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv.cold, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv.cold:
.LFSB10179:
.L98:
	.cfi_def_cfa_offset 320
	.cfi_offset 12, -40
	movq	%rbx, 264(%rsp)
	movq	%rbp, 272(%rsp)
	movq	%r13, 288(%rsp)
	movq	%r14, 296(%rsp)
	movq	%r15, 304(%rsp)
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	call	_ZSt25__throw_bad_function_callv
	.cfi_endproc
.LFE10179:
	.text
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv
	.section	.text.unlikely
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv.cold, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv.cold
.LCOLDE0:
	.text
.LHOTE0:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation:
.LFB9297:
	.cfi_startproc
	testl	%edx, %edx
	je	.L105
	cmpl	$1, %edx
	jne	.L110
	movq	%rsi, (%rdi)
.L108:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L110:
	cmpl	$2, %edx
	jne	.L108
	movdqu	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	jmp	.L108
	.p2align 4,,10
	.p2align 3
.L105:
	movq	$_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE9297:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation:
.LFB9287:
	.cfi_startproc
	testl	%edx, %edx
	je	.L112
	cmpl	$1, %edx
	jne	.L117
	movq	%rsi, (%rdi)
.L115:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L117:
	cmpl	$2, %edx
	jne	.L115
	movdqu	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	jmp	.L115
	.p2align 4,,10
	.p2align 3
.L112:
	movq	$_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE9287:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation:
.LFB9279:
	.cfi_startproc
	testl	%edx, %edx
	je	.L119
	cmpl	$1, %edx
	jne	.L124
	movq	%rsi, (%rdi)
.L122:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L124:
	cmpl	$2, %edx
	jne	.L122
	movdqu	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	jmp	.L122
	.p2align 4,,10
	.p2align 3
.L119:
	movq	$_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE9279:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation:
.LFB9268:
	.cfi_startproc
	testl	%edx, %edx
	je	.L126
	cmpl	$1, %edx
	jne	.L131
	movq	%rsi, (%rdi)
.L129:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L131:
	cmpl	$2, %edx
	jne	.L129
	movdqu	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	jmp	.L129
	.p2align 4,,10
	.p2align 3
.L126:
	movq	$_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE9268:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation:
.LFB9258:
	.cfi_startproc
	testl	%edx, %edx
	je	.L133
	cmpl	$1, %edx
	jne	.L138
	movq	%rsi, (%rdi)
.L136:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L138:
	cmpl	$2, %edx
	jne	.L136
	movdqu	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	jmp	.L136
	.p2align 4,,10
	.p2align 3
.L133:
	movq	$_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE9258:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation:
.LFB9250:
	.cfi_startproc
	testl	%edx, %edx
	je	.L140
	cmpl	$1, %edx
	jne	.L145
	movq	%rsi, (%rdi)
.L143:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L145:
	cmpl	$2, %edx
	jne	.L143
	movdqu	(%rsi), %xmm0
	movups	%xmm0, (%rdi)
	jmp	.L143
	.p2align 4,,10
	.p2align 3
.L140:
	movq	$_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE9250:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation:
.LFB8817:
	.cfi_startproc
	testl	%edx, %edx
	je	.L147
	cmpl	$1, %edx
	jne	.L152
	movq	%rsi, (%rdi)
.L150:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L152:
	cmpl	$2, %edx
	jne	.L150
	movq	(%rsi), %rax
	movq	%rax, (%rdi)
	jmp	.L150
	.p2align 4,,10
	.p2align 3
.L147:
	movq	$_ZTIZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8817:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation:
.LFB8810:
	.cfi_startproc
	testl	%edx, %edx
	je	.L154
	cmpl	$1, %edx
	jne	.L159
	movq	%rsi, (%rdi)
.L157:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L159:
	cmpl	$2, %edx
	jne	.L157
	movq	(%rsi), %rax
	movq	%rax, (%rdi)
	jmp	.L157
	.p2align 4,,10
	.p2align 3
.L154:
	movq	$_ZTIZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8810:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation:
.LFB8802:
	.cfi_startproc
	testl	%edx, %edx
	je	.L161
	cmpl	$1, %edx
	jne	.L166
	movq	%rsi, (%rdi)
.L164:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L166:
	cmpl	$2, %edx
	jne	.L164
	movq	(%rsi), %rax
	movq	%rax, (%rdi)
	jmp	.L164
	.p2align 4,,10
	.p2align 3
.L161:
	movq	$_ZTIZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8802:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation:
.LFB8791:
	.cfi_startproc
	testl	%edx, %edx
	je	.L168
	cmpl	$1, %edx
	jne	.L173
	movq	%rsi, (%rdi)
.L171:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L173:
	cmpl	$2, %edx
	jne	.L171
	movq	(%rsi), %rax
	movq	%rax, (%rdi)
	jmp	.L171
	.p2align 4,,10
	.p2align 3
.L168:
	movq	$_ZTIZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8791:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation:
.LFB8781:
	.cfi_startproc
	testl	%edx, %edx
	je	.L175
	cmpl	$1, %edx
	jne	.L180
	movq	%rsi, (%rdi)
.L178:
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L180:
	cmpl	$2, %edx
	jne	.L178
	movq	(%rsi), %rax
	movq	%rax, (%rdi)
	jmp	.L178
	.p2align 4,,10
	.p2align 3
.L175:
	movq	$_ZTIZN12_GLOBAL__N_1L10run_sharedEimbEUliE_, (%rdi)
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE8781:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.align 2
	.p2align 4
	.type	_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0, @function
_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0:
.LFB11338:
	.cfi_startproc
	testq	%rdi, %rdi
	je	.L181
	subq	%rdi, %rsi
	jmp	_ZdlPvm
.L181:
	ret
	.cfi_endproc
.LFE11338:
	.size	_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0, .-_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB8814:
	.cfi_startproc
	subq	$16, %rsp
	.cfi_def_cfa_offset 24
	movq	(%rdi), %rax
	movslq	(%rsi), %rdx
	movq	$0, 8(%rsp)
	leaq	8(%rsp), %rdi
	movq	(%rax), %rsi
	salq	$6, %rdx
	call	_ZN12_GLOBAL__N_1L12plain_kernelERmm
	movq	8(%rsp), %rax
	movq	%rax, _ZN12_GLOBAL__N_1L8p_mergedE(%rdx)
	addq	$16, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE8814:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB8807:
	.cfi_startproc
	subq	$16, %rsp
	.cfi_def_cfa_offset 24
	movq	(%rdi), %rax
	movl	(%rsi), %edx
	movq	$0, 8(%rsp)
	leaq	8(%rsp), %rdi
	movq	(%rax), %rsi
	call	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	movslq	%edx, %rax
	movq	8(%rsp), %rcx
	salq	$6, %rax
	lock addq	%rcx, _ZN12_GLOBAL__N_1L6mergedE(%rax)
	addq	$16, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE8807:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB8799:
	.cfi_startproc
	movq	(%rdi), %rax
	movslq	(%rsi), %rdx
	movq	(%rax), %rsi
	salq	$6, %rdx
	movq	%fs:0, %rax
	leaq	_ZN12_GLOBAL__N_1L8tl_plainE@tpoff(%rax), %rdi
	call	_ZN12_GLOBAL__N_1L12plain_kernelERmm
	movq	%fs:_ZN12_GLOBAL__N_1L8tl_plainE@tpoff, %rax
	movq	%rax, _ZN12_GLOBAL__N_1L8p_mergedE(%rdx)
	ret
	.cfi_endproc
.LFE8799:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.p2align 4
	.type	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, @function
_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi:
.LFB8788:
	.cfi_startproc
	movq	(%rdi), %rax
	movl	(%rsi), %edx
	movq	(%rax), %rsi
	movq	%fs:0, %rax
	leaq	_ZN12_GLOBAL__N_1L9tl_atomicE@tpoff(%rax), %rdi
	call	_ZN12_GLOBAL__N_1L13atomic_kernelERSt6atomicImEm
	movslq	%edx, %rax
	movq	%fs:_ZN12_GLOBAL__N_1L9tl_atomicE@tpoff, %rcx
	salq	$6, %rax
	lock addq	%rcx, _ZN12_GLOBAL__N_1L6mergedE(%rax)
	ret
	.cfi_endproc
.LFE8788:
	.size	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi, .-_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.section	.text._ZNSt14_Function_baseD2Ev,"axG",@progbits,_ZNSt14_Function_baseD5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt14_Function_baseD2Ev
	.type	_ZNSt14_Function_baseD2Ev, @function
_ZNSt14_Function_baseD2Ev:
.LFB6583:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA6583
	movq	16(%rdi), %rax
	testq	%rax, %rax
	je	.L195
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$3, %edx
	movq	%rdi, %rsi
	call	*%rax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L195:
	ret
	.cfi_endproc
.LFE6583:
	.section	.gcc_except_table._ZNSt14_Function_baseD2Ev,"aG",@progbits,_ZNSt14_Function_baseD5Ev,comdat
.LLSDA6583:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE6583-.LLSDACSB6583
.LLSDACSB6583:
.LLSDACSE6583:
	.section	.text._ZNSt14_Function_baseD2Ev,"axG",@progbits,_ZNSt14_Function_baseD5Ev,comdat
	.size	_ZNSt14_Function_baseD2Ev, .-_ZNSt14_Function_baseD2Ev
	.weak	_ZNSt14_Function_baseD1Ev
	.set	_ZNSt14_Function_baseD1Ev,_ZNSt14_Function_baseD2Ev
	.section	.text._ZNSt12_Vector_baseIiSaIiEED2Ev,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt12_Vector_baseIiSaIiEED2Ev
	.type	_ZNSt12_Vector_baseIiSaIiEED2Ev, @function
_ZNSt12_Vector_baseIiSaIiEED2Ev:
.LFB8456:
	.cfi_startproc
	movq	%rdi, %rax
	movq	(%rdi), %rdi
	testq	%rdi, %rdi
	je	.L198
	movq	16(%rax), %rsi
	subq	%rdi, %rsi
	jmp	_ZdlPvm
	.p2align 4,,10
	.p2align 3
.L198:
	ret
	.cfi_endproc
.LFE8456:
	.size	_ZNSt12_Vector_baseIiSaIiEED2Ev, .-_ZNSt12_Vector_baseIiSaIiEED2Ev
	.weak	_ZNSt12_Vector_baseIiSaIiEED1Ev
	.set	_ZNSt12_Vector_baseIiSaIiEED1Ev,_ZNSt12_Vector_baseIiSaIiEED2Ev
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"--threads"
.LC3:
	.string	"vector::_M_realloc_append"
.LC4:
	.string	"--runs"
.LC5:
	.string	"--n"
.LC6:
	.string	"--out"
.LC7:
	.string	"--pin"
.LC8:
	.string	"--no-warmup"
.LC9:
	.string	"--run-base"
.LC10:
	.string	"unknown argument %s\n"
.LC11:
	.string	"bad thread count\n"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC12:
	.string	"# layout: sizeof(packed atomic slot)=%zu sizeof(hdis slot)=%zu sizeof(128 slot)=%zu sizeof(64 slot)=%zu\n"
	.align 8
.LC13:
	.string	"# hardware_destructive_interference_size=%zu hardware_constructive_interference_size=%zu\n"
	.section	.rodata.str1.1
.LC14:
	.string	"# hardware_concurrency=%u\n"
.LC15:
	.string	"w"
.LC16:
	.string	"stack_local"
.LC17:
	.string	"plain"
.LC18:
	.string	"thread_local"
.LC19:
	.string	"padded_64"
.LC20:
	.string	"padded_128"
.LC21:
	.string	"padded_hdis"
.LC22:
	.string	"packed"
.LC23:
	.string	"atomic"
.LC24:
	.string	"shared_one"
.LC25:
	.string	"fopen"
	.section	.rodata.str1.8
	.align 8
.LC26:
	.string	"flavour,variant,threads,run,n_per_thread,wall_ns,ns_per_increment,checksum_ok\n"
	.section	.rodata.str1.1
.LC27:
	.string	"CHECKSUM MISMATCH %s/%s T=%d\n"
	.section	.rodata.str1.8
	.align 8
.LC28:
	.string	"%s,%s,%d,%d,%llu,%.0f,%.4f,%d\n"
	.section	.rodata.str1.1
.LC29:
	.string	"# done: %d checksum failures\n"
	.section	.text.unlikely
.LCOLDB68:
	.section	.text.startup,"ax",@progbits
.LHOTB68:
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB7742:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA7742
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pxor	%xmm0, %xmm0
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movq	%rsi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movl	%edi, %r12d
	movl	$12, %edi
	pushq	%rbx
	subq	$952, %rsp
	.cfi_offset 3, -56
	movaps	%xmm0, -752(%rbp)
	movq	$0, -736(%rbp)
.LEHB0:
	call	_Znwm
.LEHE0:
	movabsq	$8589934593, %rbx
	leaq	12(%rax), %rcx
	movl	$256, %r8d
	movq	%rax, -752(%rbp)
	movq	%rbx, (%rax)
	leaq	-688(%rbp), %rbx
	movq	%rbx, -704(%rbp)
	movabsq	$3347146957242197362, %rbx
	movq	%rbx, -688(%rbp)
	movl	$1, %ebx
	movq	%rcx, -736(%rbp)
	movl	$4, 8(%rax)
	movq	%rcx, -744(%rbp)
	movl	$11, -728(%rbp)
	movq	$10000000, -720(%rbp)
	movw	%r8w, -712(%rbp)
	movl	$1, -708(%rbp)
	movl	$1987273518, -681(%rbp)
	movb	$0, -677(%rbp)
	movq	$11, -696(%rbp)
	cmpl	$1, %r12d
	jle	.L245
	.p2align 4
	.p2align 3
.L201:
	movslq	%ebx, %r14
	movl	$.LC2, %esi
	movq	0(%r13,%r14,8), %rax
	movq	%rax, %rdi
	movq	%rax, -784(%rbp)
	call	strcmp
	testl	%eax, %eax
	jne	.L204
	leal	1(%rbx), %eax
	movl	%eax, -800(%rbp)
	cmpl	%r12d, %eax
	jl	.L371
.L204:
	movq	-784(%rbp), %rdi
	movl	$.LC4, %esi
	call	strcmp
	testl	%eax, %eax
	jne	.L220
	leal	1(%rbx), %r15d
	cmpl	%r12d, %r15d
	jl	.L372
	movq	-784(%rbp), %rdi
	movl	$.LC5, %esi
	call	strcmp
	testl	%eax, %eax
	je	.L223
.L222:
	movq	-784(%rbp), %rdi
	movl	$.LC6, %esi
	call	strcmp
	testl	%eax, %eax
	jne	.L223
	leal	1(%rbx), %r15d
	cmpl	%r12d, %r15d
	jge	.L223
	movq	8(%r13,%r14,8), %rax
	movq	%rax, %rdi
	movq	%rax, -784(%rbp)
	call	strlen
	movq	-704(%rbp), %r14
	movq	%rax, %rbx
	leaq	-688(%rbp), %rax
	cmpq	%rax, %r14
	je	.L373
	movq	-688(%rbp), %rax
	cmpq	%rbx, %rax
	jb	.L227
.L225:
	movq	-784(%rbp), %rdi
	cmpq	%r14, %rdi
	jb	.L228
	movq	-696(%rbp), %rdx
	leaq	(%r14,%rdx), %rax
	cmpq	%rdi, %rax
	jnb	.L229
.L228:
	testq	%rbx, %rbx
	je	.L231
	cmpq	$1, %rbx
	je	.L374
	movq	-784(%rbp), %rsi
	movq	%r14, %rdi
	movq	%rbx, %rdx
	call	memcpy
	movq	-704(%rbp), %r14
.L231:
	movb	$0, (%r14,%rbx)
	movq	%rbx, -696(%rbp)
	movl	%r15d, %ebx
	addl	$1, %ebx
	cmpl	%ebx, %r12d
	jg	.L201
.L377:
	movq	-752(%rbp), %rax
	movq	-744(%rbp), %rcx
	cmpq	%rax, %rcx
	jne	.L245
.L242:
	movl	$64, %r8d
	movl	$128, %ecx
	movl	$64, %edx
	xorl	%eax, %eax
	movl	$8, %esi
	movl	$.LC12, %edi
.LEHB1:
	call	printf
	movl	$64, %edx
	movl	$64, %esi
	movl	$.LC13, %edi
	xorl	%eax, %eax
	call	printf
	call	_ZNSt6thread20hardware_concurrencyEv
	movl	$.LC14, %edi
	movl	%eax, %esi
	xorl	%eax, %eax
	call	printf
	movq	-704(%rbp), %rdi
	movl	$.LC15, %esi
	call	fopen
	movq	.LC33(%rip), %xmm3
	movq	%rax, -808(%rbp)
	movhps	.LC34(%rip), %xmm3
	movq	.LC33(%rip), %xmm4
	movhps	.LC35(%rip), %xmm4
	movq	.LC33(%rip), %xmm5
	movhps	.LC36(%rip), %xmm5
	movq	.LC33(%rip), %xmm6
	movhps	.LC37(%rip), %xmm6
	movq	.LC33(%rip), %xmm7
	movhps	.LC38(%rip), %xmm7
	movq	.LC33(%rip), %xmm2
	movhps	.LC39(%rip), %xmm2
	movq	.LC40(%rip), %xmm8
	movhps	.LC34(%rip), %xmm8
	movq	.LC40(%rip), %xmm9
	movq	.LC40(%rip), %xmm10
	movhps	.LC35(%rip), %xmm9
	movhps	.LC41(%rip), %xmm10
	movq	.LC40(%rip), %xmm11
	movq	.LC40(%rip), %xmm12
	movhps	.LC36(%rip), %xmm11
	movhps	.LC37(%rip), %xmm12
	movq	.LC40(%rip), %xmm13
	movq	.LC40(%rip), %xmm0
	movhps	.LC39(%rip), %xmm0
	movhps	.LC38(%rip), %xmm13
	testq	%rax, %rax
	je	.L365
	movl	$78, %edx
	movl	$1, %esi
	movq	-808(%rbp), %rcx
	movl	$.LC26, %edi
	movaps	%xmm10, -992(%rbp)
	movaps	%xmm2, -976(%rbp)
	movaps	%xmm3, -960(%rbp)
	movaps	%xmm8, -944(%rbp)
	movaps	%xmm4, -928(%rbp)
	movaps	%xmm0, -912(%rbp)
	movaps	%xmm5, -896(%rbp)
	movaps	%xmm7, -880(%rbp)
	movaps	%xmm13, -864(%rbp)
	movaps	%xmm6, -848(%rbp)
	movaps	%xmm9, -832(%rbp)
	movaps	%xmm12, -800(%rbp)
	movaps	%xmm11, -784(%rbp)
	call	fwrite
.LEHE1:
	movdqa	-912(%rbp), %xmm0
	pxor	%xmm1, %xmm1
	movdqa	-864(%rbp), %xmm13
	movl	$624, %edi
	movdqa	-800(%rbp), %xmm12
	movaps	%xmm1, -656(%rbp)
	movdqa	-784(%rbp), %xmm11
	movdqa	-992(%rbp), %xmm10
	movaps	%xmm0, -672(%rbp)
	movdqa	-832(%rbp), %xmm9
	movq	.LC42(%rip), %xmm0
	movaps	%xmm13, -624(%rbp)
	movhps	.LC43(%rip), %xmm0
	movaps	%xmm1, -608(%rbp)
	movaps	%xmm0, -640(%rbp)
	movq	.LC44(%rip), %xmm0
	movhps	.LC45(%rip), %xmm0
	movaps	%xmm12, -576(%rbp)
	movaps	%xmm0, -592(%rbp)
	movq	.LC46(%rip), %xmm0
	movhps	.LC47(%rip), %xmm0
	movaps	%xmm1, -560(%rbp)
	movaps	%xmm0, -544(%rbp)
	movq	.LC48(%rip), %xmm0
	movhps	.LC49(%rip), %xmm0
	movaps	%xmm11, -528(%rbp)
	movaps	%xmm0, -496(%rbp)
	movq	.LC50(%rip), %xmm0
	movhps	.LC51(%rip), %xmm0
	movaps	%xmm1, -512(%rbp)
	movaps	%xmm0, -448(%rbp)
	movaps	%xmm10, -480(%rbp)
	movaps	%xmm1, -464(%rbp)
	movaps	%xmm9, -432(%rbp)
	movaps	%xmm1, -416(%rbp)
	movdqa	-976(%rbp), %xmm2
	movdqa	-944(%rbp), %xmm8
	movdqa	-880(%rbp), %xmm7
	movaps	%xmm1, -368(%rbp)
	movdqa	-848(%rbp), %xmm6
	movdqa	-896(%rbp), %xmm5
	movaps	%xmm8, -384(%rbp)
	movq	.LC52(%rip), %xmm0
	movhps	.LC53(%rip), %xmm0
	movaps	%xmm2, -336(%rbp)
	movaps	%xmm0, -400(%rbp)
	movq	.LC54(%rip), %xmm0
	movhps	.LC55(%rip), %xmm0
	movaps	%xmm1, -320(%rbp)
	movaps	%xmm0, -352(%rbp)
	movq	.LC56(%rip), %xmm0
	movhps	.LC57(%rip), %xmm0
	movaps	%xmm7, -288(%rbp)
	movaps	%xmm0, -304(%rbp)
	movq	.LC58(%rip), %xmm0
	movhps	.LC59(%rip), %xmm0
	movaps	%xmm1, -272(%rbp)
	movaps	%xmm0, -256(%rbp)
	movq	.LC60(%rip), %xmm0
	movhps	.LC61(%rip), %xmm0
	movaps	%xmm6, -240(%rbp)
	movaps	%xmm0, -208(%rbp)
	movq	.LC62(%rip), %xmm0
	movhps	.LC63(%rip), %xmm0
	movaps	%xmm1, -224(%rbp)
	movaps	%xmm5, -192(%rbp)
	movaps	%xmm1, -176(%rbp)
	movaps	%xmm0, -160(%rbp)
	movdqa	-928(%rbp), %xmm4
	movdqa	-960(%rbp), %xmm3
	movq	.LC64(%rip), %xmm0
	movaps	%xmm1, -128(%rbp)
	movhps	.LC65(%rip), %xmm0
	movaps	%xmm4, -144(%rbp)
	movaps	%xmm0, -112(%rbp)
	movq	.LC66(%rip), %xmm0
	movhps	.LC67(%rip), %xmm0
	movaps	%xmm3, -96(%rbp)
	movaps	%xmm1, -80(%rbp)
	movaps	%xmm0, -64(%rbp)
.LEHB2:
	call	_Znwm
.LEHE2:
	movq	%rax, -848(%rbp)
	movq	%rax, %r12
	leaq	624(%rax), %rax
	pxor	%xmm4, %xmm4
	movq	%rax, -864(%rbp)
	leaq	-672(%rbp), %rbx
	leaq	-656(%rbp), %r15
	leaq	-32(%rbp), %r13
	movaps	%xmm4, -784(%rbp)
	.p2align 4
	.p2align 3
.L251:
	movdqa	-16(%r15), %xmm0
	movq	16(%r15), %rax
	movq	$0, 32(%r12)
	movdqa	-784(%rbp), %xmm3
	movq	$0, 40(%r12)
	movups	%xmm0, (%r12)
	movups	%xmm3, 16(%r12)
	testq	%rax, %rax
	je	.L250
	leaq	16(%r12), %rdi
	movl	$2, %edx
	movq	%r15, %rsi
	movq	%rdi, %r14
.LEHB3:
	call	*%rax
.LEHE3:
	movdqa	16(%r15), %xmm0
	movups	%xmm0, 32(%r12)
.L250:
	addq	$48, %r15
	addq	$48, %r12
	cmpq	%r15, %r13
	jne	.L251
	leaq	576(%rbx), %r15
	jmp	.L258
	.p2align 4,,10
	.p2align 3
.L289:
	subq	$48, %r15
.L258:
	movq	32(%r15), %rax
	testq	%rax, %rax
	je	.L257
	leaq	16(%r15), %rdi
	movl	$3, %edx
	movq	%rdi, %rsi
	call	*%rax
.L257:
	cmpq	%r15, %rbx
	jne	.L289
	movzbl	-711(%rbp), %eax
	xorl	%r13d, %r13d
	xorl	$1, %eax
	movzbl	%al, %eax
	movl	%eax, -784(%rbp)
	cmpl	%eax, -728(%rbp)
	jl	.L260
	.p2align 4
	.p2align 3
.L259:
	movq	-744(%rbp), %rax
	movq	-752(%rbp), %r15
	movq	%rax, -832(%rbp)
	cmpq	%r15, %rax
	je	.L261
	.p2align 4
	.p2align 3
.L270:
	movq	-848(%rbp), %r14
	cmpq	%r12, %r14
	je	.L262
	movl	(%r15), %ebx
	jmp	.L269
	.p2align 4,,10
	.p2align 3
.L375:
	movl	-784(%rbp), %esi
	movl	$1, %edi
	testl	%esi, %esi
	je	.L265
	testq	%rax, %rax
	js	.L267
.L376:
	pxor	%xmm1, %xmm1
	cvtsi2sdq	%rax, %xmm1
.L268:
	movapd	%xmm0, %xmm2
	movl	-784(%rbp), %esi
	movq	(%r14), %rdx
	movl	%ebx, %r8d
	divsd	%xmm1, %xmm2
	addl	-708(%rbp), %esi
	movq	8(%r14), %rcx
	pushq	%rdi
	pushq	%rax
	movq	-808(%rbp), %rdi
	leal	-1(%rsi), %r9d
	movl	$2, %eax
	movl	$.LC28, %esi
	movapd	%xmm2, %xmm1
.LEHB4:
	.cfi_escape 0x2e,0x10
	call	fprintf
	popq	%rax
	popq	%rdx
.L265:
	addq	$48, %r14
	cmpq	%r14, %r12
	je	.L262
.L269:
	movq	-720(%rbp), %rax
	movl	%ebx, -764(%rbp)
	movq	%rax, -760(%rbp)
	movzbl	-712(%rbp), %eax
	movb	%al, -765(%rbp)
	cmpq	$0, 32(%r14)
	je	.L366
	leaq	-760(%rbp), %rdx
	leaq	-764(%rbp), %rsi
	leaq	16(%r14), %rdi
	leaq	-765(%rbp), %rcx
	.cfi_escape 0x2e,0
	call	*40(%r14)
	movq	%rax, %rcx
	movq	-720(%rbp), %rax
	movslq	%ebx, %rdx
	imulq	%rax, %rdx
	cmpq	%rcx, %rdx
	je	.L375
	movq	8(%r14), %rcx
	movq	(%r14), %rdx
	movl	%ebx, %r8d
	movl	$.LC27, %esi
	movq	stderr(%rip), %rdi
	xorl	%eax, %eax
	movsd	%xmm0, -800(%rbp)
	addl	$1, %r13d
	call	fprintf
.LEHE4:
	movl	-784(%rbp), %ecx
	testl	%ecx, %ecx
	je	.L265
	movq	-720(%rbp), %rax
	movsd	-800(%rbp), %xmm0
	xorl	%edi, %edi
	testq	%rax, %rax
	jns	.L376
.L267:
	movq	%rax, %rdx
	movq	%rax, %rcx
	pxor	%xmm1, %xmm1
	shrq	%rdx
	andl	$1, %ecx
	orq	%rcx, %rdx
	cvtsi2sdq	%rdx, %xmm1
	addsd	%xmm1, %xmm1
	jmp	.L268
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L243:
	addq	$4, %rax
	cmpq	%rax, %rcx
	je	.L242
.L245:
	movl	(%rax), %ebx
	leal	-1(%rbx), %edx
	cmpl	$15, %edx
	jbe	.L243
	movl	$17, %edx
	movl	$1, %esi
	movl	$.LC11, %edi
	movq	stderr(%rip), %rcx
.LEHB5:
	call	fwrite
.LEHE5:
.L241:
	movl	$2, %r15d
.L244:
	movq	-704(%rbp), %rdi
	leaq	-688(%rbp), %rax
	cmpq	%rax, %rdi
	je	.L274
	movq	-688(%rbp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm
.L274:
	movq	-752(%rbp), %rdi
	testq	%rdi, %rdi
	je	.L200
	movq	-736(%rbp), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm
.L200:
	leaq	-40(%rbp), %rsp
	movl	%r15d, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L223:
	.cfi_restore_state
	movq	-784(%rbp), %rdi
	movl	$.LC7, %esi
	call	strcmp
	testl	%eax, %eax
	jne	.L237
	movb	$1, -712(%rbp)
.L219:
	addl	$1, %ebx
	cmpl	%ebx, %r12d
	jg	.L201
	jmp	.L377
	.p2align 4,,10
	.p2align 3
.L220:
	movq	-784(%rbp), %rdi
	movl	$.LC5, %esi
	call	strcmp
	testl	%eax, %eax
	jne	.L222
	leal	1(%rbx), %r15d
	cmpl	%r12d, %r15d
	jge	.L223
	movq	8(%r13,%r14,8), %rdi
	movl	$10, %edx
	xorl	%esi, %esi
	movl	%r15d, %ebx
	call	__isoc23_strtoull
	movq	%rax, -720(%rbp)
	jmp	.L219
	.p2align 4,,10
	.p2align 3
.L237:
	movq	-784(%rbp), %rdi
	movl	$.LC8, %esi
	call	strcmp
	testl	%eax, %eax
	jne	.L239
	movb	$0, -711(%rbp)
	jmp	.L219
	.p2align 4,,10
	.p2align 3
.L371:
	movq	8(%r13,%r14,8), %r15
	movl	$0, %ebx
	movl	$0, %r14d
	movq	$0, -784(%rbp)
	cmpb	$0, (%r15)
	je	.L205
	.p2align 4
	.p2align 3
.L215:
	xorl	%esi, %esi
	movl	$10, %edx
	movq	%r15, %rdi
	call	__isoc23_strtol
	movq	%rax, %rsi
	cmpq	%rbx, -784(%rbp)
	je	.L206
	movl	%eax, (%rbx)
	addq	$4, %rbx
.L207:
	movzbl	(%r15), %eax
	cmpb	$44, %al
	je	.L211
	testb	%al, %al
	je	.L211
	.p2align 5
	.p2align 4
	.p2align 3
.L212:
	movzbl	1(%r15), %eax
	addq	$1, %r15
	testb	%al, %al
	je	.L211
	cmpb	$44, %al
	jne	.L212
.L211:
	cmpb	$44, %al
	sete	%al
	movzbl	%al, %eax
	addq	%rax, %r15
	cmpb	$0, (%r15)
	jne	.L215
.L205:
	movq	-752(%rbp), %rdi
	movq	-784(%rbp), %rax
	movq	%r14, -752(%rbp)
	movq	-736(%rbp), %rsi
	movq	%rbx, -744(%rbp)
	movq	%rax, -736(%rbp)
	testq	%rdi, %rdi
	je	.L218
	subq	%rdi, %rsi
	call	_ZdlPvm
.L218:
	movl	-800(%rbp), %ebx
	jmp	.L219
	.p2align 4,,10
	.p2align 3
.L206:
	movq	-784(%rbp), %r8
	movabsq	$2305843009213693951, %rax
	subq	%r14, %r8
	movq	%r8, %rdx
	sarq	$2, %rdx
	cmpq	%rax, %rdx
	je	.L364
	testq	%rdx, %rdx
	movl	$1, %eax
	movq	%rsi, -848(%rbp)
	cmovne	%rdx, %rax
	movq	%r8, -832(%rbp)
	addq	%rdx, %rax
	movabsq	$2305843009213693951, %rdx
	cmpq	%rdx, %rax
	cmova	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdi
	movq	%rax, -808(%rbp)
.LEHB6:
	call	_Znwm
.LEHE6:
	movq	-832(%rbp), %r8
	movq	-848(%rbp), %rsi
	movq	%rax, %r9
	movl	%esi, (%rax,%r8)
	testq	%r8, %r8
	je	.L209
	movq	%r8, %rdx
	movq	%r14, %rsi
	movq	%rax, %rdi
	call	memcpy
	movq	-832(%rbp), %r8
	movq	%rax, %r9
.L209:
	leaq	4(%r9,%r8), %rbx
	testq	%r14, %r14
	je	.L210
	movq	-784(%rbp), %rsi
	movq	%r14, %rdi
	movq	%r9, -832(%rbp)
	subq	%r14, %rsi
	call	_ZdlPvm
	movq	-832(%rbp), %r9
.L210:
	movq	-808(%rbp), %rax
	movq	%r9, %r14
	addq	%r9, %rax
	movq	%rax, -784(%rbp)
	jmp	.L207
	.p2align 4,,10
	.p2align 3
.L372:
	movq	8(%r13,%r14,8), %rdi
	movl	$10, %edx
	xorl	%esi, %esi
	movl	%r15d, %ebx
	call	__isoc23_strtol
	movl	%eax, -728(%rbp)
	jmp	.L219
	.p2align 4,,10
	.p2align 3
.L239:
	movq	-784(%rbp), %rdi
	movl	$.LC9, %esi
	call	strcmp
	testl	%eax, %eax
	jne	.L240
	addl	$1, %ebx
	cmpl	%r12d, %ebx
	jge	.L240
	movq	8(%r13,%r14,8), %rdi
	movl	$10, %edx
	xorl	%esi, %esi
	call	__isoc23_strtol
	movl	%eax, -708(%rbp)
	jmp	.L219
.L227:
	leaq	(%rax,%rax), %rcx
	cmpq	%rcx, %rbx
	jnb	.L282
	movabsq	$9223372036854775806, %rax
	cmpq	%rcx, %rax
	jnb	.L281
	movabsq	$9223372036854775807, %rdi
	movq	%rax, %rcx
	jmp	.L234
.L374:
	movq	-784(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, (%r14)
	movq	-704(%rbp), %r14
	jmp	.L231
	.p2align 4,,10
	.p2align 3
.L262:
	addq	$4, %r15
	cmpq	%r15, -832(%rbp)
	jne	.L270
.L261:
	addl	$1, -784(%rbp)
	movl	-784(%rbp), %eax
	cmpl	%eax, -728(%rbp)
	jge	.L259
.L260:
	movq	-808(%rbp), %rdi
.LEHB7:
	call	fclose
	movl	%r13d, %esi
	movl	$.LC29, %edi
	xorl	%eax, %eax
	call	printf
.LEHE7:
	movq	-848(%rbp), %rbx
	xorl	%r15d, %r15d
	testl	%r13d, %r13d
	setne	%r15b
	cmpq	%r12, %rbx
	je	.L271
	.p2align 4
	.p2align 3
.L273:
	movq	32(%rbx), %rax
	testq	%rax, %rax
	je	.L272
	leaq	16(%rbx), %rdi
	movl	$3, %edx
	movq	%rdi, %rsi
	call	*%rax
.L272:
	addq	$48, %rbx
	cmpq	%rbx, %r12
	jne	.L273
.L271:
	movq	-864(%rbp), %rsi
	movq	-848(%rbp), %rdi
	call	_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0
	jmp	.L244
.L373:
	cmpq	$15, %rbx
	jbe	.L225
	cmpq	$29, %rbx
	ja	.L282
	movl	$30, %ecx
.L281:
	leaq	1(%rcx), %rdi
	jmp	.L234
.L282:
	leaq	1(%rbx), %rdi
	movq	%rbx, %rcx
.L234:
	movq	%rcx, -808(%rbp)
.LEHB8:
	call	_Znwm
	testq	%rbx, %rbx
	movq	%rax, -800(%rbp)
	movq	-808(%rbp), %rcx
	je	.L235
	movq	-784(%rbp), %rsi
	movq	%rbx, %rdx
	movq	%rax, %rdi
	call	memcpy
	movq	-808(%rbp), %rcx
.L235:
	leaq	-688(%rbp), %rax
	cmpq	%rax, %r14
	je	.L236
	movq	-688(%rbp), %rax
	movq	%r14, %rdi
	movq	%rcx, -784(%rbp)
	leaq	1(%rax), %rsi
	call	_ZdlPvm
	movq	-784(%rbp), %rcx
.L236:
	movq	-800(%rbp), %rax
	movq	%rcx, -688(%rbp)
	movq	%rax, -704(%rbp)
	movq	%rax, %r14
	jmp	.L231
.L240:
	movq	-784(%rbp), %rdx
	movq	stderr(%rip), %rdi
	movl	$.LC10, %esi
	xorl	%eax, %eax
	call	fprintf
.LEHE8:
	jmp	.L241
.L296:
	movq	%rax, %r13
	jmp	.L248
.L297:
	movq	%rax, %r13
	jmp	.L253
.L358:
	jmp	.L359
.L362:
	jmp	.L363
.L293:
	movq	%rax, %rbx
	jmp	.L276
.L294:
	movq	%rax, %rbx
	jmp	.L203
.L360:
	jmp	.L361
	.section	.gcc_except_table,"a",@progbits
.LLSDA7742:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE7742-.LLSDACSB7742
.LLSDACSB7742:
	.uleb128 .LEHB0-.LFB7742
	.uleb128 .LEHE0-.LEHB0
	.uleb128 .L294-.LFB7742
	.uleb128 0
	.uleb128 .LEHB1-.LFB7742
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L358-.LFB7742
	.uleb128 0
	.uleb128 .LEHB2-.LFB7742
	.uleb128 .LEHE2-.LEHB2
	.uleb128 .L296-.LFB7742
	.uleb128 0
	.uleb128 .LEHB3-.LFB7742
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L297-.LFB7742
	.uleb128 0
	.uleb128 .LEHB4-.LFB7742
	.uleb128 .LEHE4-.LEHB4
	.uleb128 .L362-.LFB7742
	.uleb128 0
	.uleb128 .LEHB5-.LFB7742
	.uleb128 .LEHE5-.LEHB5
	.uleb128 .L358-.LFB7742
	.uleb128 0
	.uleb128 .LEHB6-.LFB7742
	.uleb128 .LEHE6-.LEHB6
	.uleb128 .L360-.LFB7742
	.uleb128 0
	.uleb128 .LEHB7-.LFB7742
	.uleb128 .LEHE7-.LEHB7
	.uleb128 .L293-.LFB7742
	.uleb128 0
	.uleb128 .LEHB8-.LFB7742
	.uleb128 .LEHE8-.LEHB8
	.uleb128 .L358-.LFB7742
	.uleb128 0
.LLSDACSE7742:
	.section	.text.startup
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC7742
	.type	main.cold, @function
main.cold:
.LFSB7742:
.L229:
	.cfi_def_cfa 6, 16
	.cfi_offset 3, -56
	.cfi_offset 6, -16
	.cfi_offset 12, -48
	.cfi_offset 13, -40
	.cfi_offset 14, -32
	.cfi_offset 15, -24
	movq	-784(%rbp), %rcx
	xorl	%r9d, %r9d
	movq	%rbx, %r8
	movq	%r14, %rsi
	leaq	-704(%rbp), %rdi
.LEHB9:
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE15_M_replace_coldEPcmPKcmm
.LEHE9:
	movq	-704(%rbp), %r14
	jmp	.L231
.L292:
.L359:
	movq	%rax, %rbx
.L217:
	movq	-704(%rbp), %rdi
	leaq	-688(%rbp), %rax
	cmpq	%rax, %rdi
	je	.L280
	movq	-688(%rbp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm
.L280:
	leaq	-752(%rbp), %rdi
	call	_ZNSt12_Vector_baseIiSaIiEED2Ev
	movq	%rbx, %rdi
.LEHB10:
	call	_Unwind_Resume
.LEHE10:
.L248:
	xorl	%edi, %edi
	leaq	-672(%rbp), %rbx
	movq	%rdi, -864(%rbp)
	movq	%rdi, -848(%rbp)
.L249:
	movq	-864(%rbp), %rsi
	movq	-848(%rbp), %rdi
	leaq	-48(%rbp), %r15
	call	_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0
.L256:
	subq	$48, %r15
	leaq	16(%r15), %rdi
	call	_ZNSt14_Function_baseD2Ev
	cmpq	%rbx, %r15
	jne	.L256
	movq	%r13, %rbx
	jmp	.L217
.L253:
	movq	%r14, %rdi
	call	_ZNSt14_Function_baseD2Ev
	cmpq	%r12, -848(%rbp)
	je	.L249
	movq	-848(%rbp), %rax
.L255:
	leaq	16(%rax), %rdi
	leaq	48(%rax), %r15
	call	_ZNSt14_Function_baseD2Ev
	movq	%r15, %rax
	cmpq	%r12, %r15
	je	.L249
	jmp	.L255
.L364:
	movl	$.LC3, %edi
.LEHB11:
	call	_ZSt20__throw_length_errorPKc
.LEHE11:
.L295:
.L361:
	movq	%rbx, -664(%rbp)
	movq	%rax, %rbx
	movq	-784(%rbp), %rax
	leaq	-672(%rbp), %rdi
	movq	%r14, -672(%rbp)
	movq	%rax, -656(%rbp)
	call	_ZNSt12_Vector_baseIiSaIiEED2Ev
	jmp	.L217
.L366:
.LEHB12:
	call	_ZSt25__throw_bad_function_callv
.LEHE12:
.L298:
.L363:
	movq	%rax, %rbx
.L284:
	movq	-848(%rbp), %r15
.L279:
	movq	32(%r15), %rax
	testq	%rax, %rax
	je	.L278
	leaq	16(%r15), %rdi
	movl	$3, %edx
	movq	%rdi, %rsi
	call	*%rax
.L278:
	leaq	48(%r15), %rax
	movq	%rax, %r15
	cmpq	%rax, %r12
	jne	.L279
.L277:
	movq	-864(%rbp), %rsi
	movq	-848(%rbp), %rdi
	call	_ZNSt12_Vector_baseIN12_GLOBAL__N_17VariantESaIS1_EED2Ev.isra.0
	jmp	.L217
.L365:
	movl	$.LC25, %edi
.LEHB13:
	call	perror
.LEHE13:
	jmp	.L241
.L276:
	cmpq	%r12, -848(%rbp)
	jne	.L284
	jmp	.L277
.L203:
	leaq	-752(%rbp), %rdi
	call	_ZNSt12_Vector_baseIiSaIiEED2Ev
	movq	%rbx, %rdi
.LEHB14:
	call	_Unwind_Resume
.LEHE14:
	.cfi_endproc
.LFE7742:
	.section	.gcc_except_table
.LLSDAC7742:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC7742-.LLSDACSBC7742
.LLSDACSBC7742:
	.uleb128 .LEHB9-.LCOLDB68
	.uleb128 .LEHE9-.LEHB9
	.uleb128 .L292-.LCOLDB68
	.uleb128 0
	.uleb128 .LEHB10-.LCOLDB68
	.uleb128 .LEHE10-.LEHB10
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB11-.LCOLDB68
	.uleb128 .LEHE11-.LEHB11
	.uleb128 .L295-.LCOLDB68
	.uleb128 0
	.uleb128 .LEHB12-.LCOLDB68
	.uleb128 .LEHE12-.LEHB12
	.uleb128 .L298-.LCOLDB68
	.uleb128 0
	.uleb128 .LEHB13-.LCOLDB68
	.uleb128 .LEHE13-.LEHB13
	.uleb128 .L292-.LCOLDB68
	.uleb128 0
	.uleb128 .LEHB14-.LCOLDB68
	.uleb128 .LEHE14-.LEHB14
	.uleb128 0
	.uleb128 0
.LLSDACSEC7742:
	.section	.text.unlikely
	.section	.text.startup
	.size	main, .-main
	.section	.text.unlikely
	.size	main.cold, .-main.cold
.LCOLDE68:
	.section	.text.startup
.LHOTE68:
	.section	.text._ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev,"axG",@progbits,_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev
	.type	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev, @function
_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev:
.LFB9881:
	.cfi_startproc
	movq	(%rdi), %rdi
	testq	%rdi, %rdi
	je	.L378
	movq	(%rdi), %rax
	jmp	*8(%rax)
	.p2align 4,,10
	.p2align 3
.L378:
	ret
	.cfi_endproc
.LFE9881:
	.size	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev, .-_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev
	.weak	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev
	.set	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev,_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev
	.section	.rodata.str1.1
.LC69:
	.string	"vector::reserve"
	.section	.text.unlikely
.LCOLDB70:
	.text
.LHOTB70:
	.p2align 4
	.type	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE, @function
_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE:
.LFB7543:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA7543
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movslq	%edi, %r14
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%r14, %rax
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$88, %rsp
	.cfi_def_cfa_offset 144
	shrq	$60, %rax
	movb	%sil, 63(%rsp)
	movq	%rdx, 8(%rsp)
	movl	$0, 68(%rsp)
	movb	$0, 67(%rsp)
	jne	.L456
	movq	%r14, %r12
	testq	%r14, %r14
	je	.L423
	salq	$3, %r14
	movq	%r14, %rdi
.LEHB15:
	call	_Znwm
.LEHE15:
	movq	%rax, %rbx
	addq	%rax, %r14
	movq	%rax, %r13
	xorl	%ebp, %ebp
	leaq	68(%rsp), %rax
	leaq	63(%rsp), %rdi
	movq	%rdi, %xmm3
	movq	%rax, %xmm4
	punpcklqdq	%xmm4, %xmm3
	movaps	%xmm3, 16(%rsp)
	jmp	.L396
	.p2align 4,,10
	.p2align 3
.L462:
	movq	$0, (%rbx)
	movl	$48, %edi
.LEHB16:
	call	_Znwm
.LEHE16:
	leaq	67(%rsp), %rsi
	movl	%ebp, 8(%rax)
	movl	$_ZNSt6thread24_M_thread_deps_never_runEv, %edx
	movq	%rbx, %rdi
	movdqa	16(%rsp), %xmm1
	movq	%rsi, %xmm0
	movq	%rax, 72(%rsp)
	movhps	8(%rsp), %xmm0
	movq	$_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE+16, (%rax)
	movups	%xmm1, 16(%rax)
	movups	%xmm0, 32(%rax)
	leaq	72(%rsp), %rax
	movq	%rax, %rsi
	movq	%rax, 32(%rsp)
.LEHB17:
	call	_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE
.LEHE17:
	movq	72(%rsp), %rdi
	testq	%rdi, %rdi
	je	.L384
	movq	(%rdi), %rax
	call	*8(%rax)
.L384:
	addl	$1, %ebp
	addq	$8, %rbx
	cmpl	%ebp, %r12d
	jle	.L397
.L396:
	cmpq	%rbx, %r14
	jne	.L462
	movabsq	$1152921504606846975, %rax
	movq	%r14, %rcx
	subq	%r13, %rcx
	movq	%rcx, %rdx
	sarq	$3, %rdx
	cmpq	%rax, %rdx
	je	.L457
	testq	%rdx, %rdx
	movl	$1, %eax
	movq	%rcx, 32(%rsp)
	cmovne	%rdx, %rax
	addq	%rdx, %rax
	movabsq	$1152921504606846975, %rdx
	cmpq	%rdx, %rax
	cmova	%rdx, %rax
	salq	$3, %rax
	movq	%rax, %rdi
	movq	%rax, 40(%rsp)
.LEHB18:
	call	_Znwm
.LEHE18:
	movq	32(%rsp), %rcx
	movl	$48, %edi
	movq	%rax, %r15
	addq	%rax, %rcx
	movq	$0, (%rcx)
	movq	%rcx, 48(%rsp)
.LEHB19:
	call	_Znwm
.LEHE19:
	movdqa	16(%rsp), %xmm2
	leaq	67(%rsp), %rsi
	movl	%ebp, 8(%rax)
	movl	$_ZNSt6thread24_M_thread_deps_never_runEv, %edx
	movq	%rsi, %xmm0
	movq	%rax, 72(%rsp)
	movhps	8(%rsp), %xmm0
	movq	48(%rsp), %rdi
	movq	$_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE+16, (%rax)
	movups	%xmm2, 16(%rax)
	movups	%xmm0, 32(%rax)
	leaq	72(%rsp), %rax
	movq	%rax, %rsi
	movq	%rax, 32(%rsp)
.LEHB20:
	call	_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE
.LEHE20:
	movq	72(%rsp), %rdi
	testq	%rdi, %rdi
	je	.L389
	movq	(%rdi), %rax
	call	*8(%rax)
.L389:
	cmpq	%rbx, %r13
	je	.L463
	movq	%rbx, %rsi
	movq	%r13, %rdx
	movq	%r15, %rax
	subq	%r13, %rsi
	addq	%r15, %rsi
	.p2align 5
	.p2align 4
	.p2align 3
.L394:
	movq	(%rdx), %rcx
	addq	$8, %rax
	addq	$8, %rdx
	movq	%rcx, -8(%rax)
	cmpq	%rsi, %rax
	jne	.L394
	movq	%r15, %rax
	subq	%r13, %rax
	leaq	8(%rbx,%rax), %rbx
	testq	%r13, %r13
	je	.L395
.L391:
	movq	%r14, %rsi
	movq	%r13, %rdi
	subq	%r13, %rsi
	call	_ZdlPvm
.L395:
	movq	40(%rsp), %r14
	addl	$1, %ebp
	movq	%r15, %r13
	addq	%r15, %r14
	cmpl	%ebp, %r12d
	jg	.L396
	.p2align 4
	.p2align 4
	.p2align 3
.L397:
	movl	68(%rsp), %eax
	cmpl	%eax, %r12d
	jg	.L397
.L464:
	call	_ZNSt6chrono3_V212steady_clock3nowEv
	movq	%rax, %rbp
	movb	$1, 67(%rsp)
	cmpq	%r13, %rbx
	je	.L398
	movq	%r13, %r15
	.p2align 4
	.p2align 3
.L402:
	movq	%r15, %rdi
.LEHB21:
	call	_ZNSt6thread4joinEv
.LEHE21:
	addq	$8, %r15
	cmpq	%r15, %rbx
	jne	.L402
	testl	%r12d, %r12d
	je	.L403
.L417:
	xorl	%eax, %eax
	xorl	%edx, %edx
	.p2align 5
	.p2align 4
	.p2align 3
.L404:
	movq	%rax, %rcx
	salq	$8, %rcx
	movq	_ZN12_GLOBAL__N_1L4endsE(%rcx), %rcx
	cmpq	%rcx, %rdx
	cmovb	%rcx, %rdx
	addq	$1, %rax
	cmpl	%eax, %r12d
	jg	.L404
	subq	%rbp, %rdx
	js	.L405
	pxor	%xmm0, %xmm0
	cvtsi2sdq	%rdx, %xmm0
.L406:
	cmpq	%r13, %rbx
	je	.L407
.L422:
	movq	%r13, %rax
	.p2align 4
	.p2align 4
	.p2align 3
.L409:
	cmpq	$0, (%rax)
	jne	.L412
	addq	$8, %rax
	cmpq	%rax, %rbx
	jne	.L409
.L407:
	testq	%r13, %r13
	je	.L380
	movq	%r14, %rsi
	movq	%r13, %rdi
	movsd	%xmm0, 8(%rsp)
	subq	%r13, %rsi
	call	_ZdlPvm
	movsd	8(%rsp), %xmm0
.L380:
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L423:
	.cfi_restore_state
	movl	68(%rsp), %eax
	xorl	%r14d, %r14d
	xorl	%ebx, %ebx
	xorl	%r13d, %r13d
	cmpl	%eax, %r12d
	jg	.L397
	jmp	.L464
	.p2align 4,,10
	.p2align 3
.L405:
	movq	%rdx, %rax
	andl	$1, %edx
	pxor	%xmm0, %xmm0
	shrq	%rax
	orq	%rdx, %rax
	cvtsi2sdq	%rax, %xmm0
	addsd	%xmm0, %xmm0
	jmp	.L406
	.p2align 4,,10
	.p2align 3
.L463:
	leaq	8(%r15), %rbx
	jmp	.L391
.L398:
	testl	%r12d, %r12d
	jne	.L417
	negq	%rbp
	js	.L415
	pxor	%xmm0, %xmm0
	cvtsi2sdq	%rbp, %xmm0
	jmp	.L407
	.p2align 4,,10
	.p2align 3
.L403:
	negq	%rbp
	js	.L420
	pxor	%xmm0, %xmm0
	cvtsi2sdq	%rbp, %xmm0
	jmp	.L422
.L420:
	movq	%rbp, %rax
	andl	$1, %ebp
	pxor	%xmm0, %xmm0
	shrq	%rax
	orq	%rbp, %rax
	cvtsi2sdq	%rax, %xmm0
	addsd	%xmm0, %xmm0
	jmp	.L422
.L415:
	movq	%rbp, %rax
	andl	$1, %ebp
	pxor	%xmm0, %xmm0
	shrq	%rax
	orq	%rbp, %rax
	cvtsi2sdq	%rax, %xmm0
	addsd	%xmm0, %xmm0
	jmp	.L407
.L424:
	movq	%rax, %rbp
	jmp	.L461
.L454:
	jmp	.L455
.L427:
	movq	%rax, %rbp
	jmp	.L392
.L425:
	movq	%rax, %rbp
	jmp	.L386
.L452:
	jmp	.L453
.L426:
	movq	%rax, %rbp
	jmp	.L393
	.section	.gcc_except_table
.LLSDA7543:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE7543-.LLSDACSB7543
.LLSDACSB7543:
	.uleb128 .LEHB15-.LFB7543
	.uleb128 .LEHE15-.LEHB15
	.uleb128 .L452-.LFB7543
	.uleb128 0
	.uleb128 .LEHB16-.LFB7543
	.uleb128 .LEHE16-.LEHB16
	.uleb128 .L454-.LFB7543
	.uleb128 0
	.uleb128 .LEHB17-.LFB7543
	.uleb128 .LEHE17-.LEHB17
	.uleb128 .L425-.LFB7543
	.uleb128 0
	.uleb128 .LEHB18-.LFB7543
	.uleb128 .LEHE18-.LEHB18
	.uleb128 .L454-.LFB7543
	.uleb128 0
	.uleb128 .LEHB19-.LFB7543
	.uleb128 .LEHE19-.LEHB19
	.uleb128 .L426-.LFB7543
	.uleb128 0
	.uleb128 .LEHB20-.LFB7543
	.uleb128 .LEHE20-.LEHB20
	.uleb128 .L427-.LFB7543
	.uleb128 0
	.uleb128 .LEHB21-.LFB7543
	.uleb128 .LEHE21-.LEHB21
	.uleb128 .L424-.LFB7543
	.uleb128 0
.LLSDACSE7543:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC7543
	.type	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE.cold, @function
_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE.cold:
.LFSB7543:
.L457:
	.cfi_def_cfa_offset 144
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movl	$.LC3, %edi
.LEHB22:
	call	_ZSt20__throw_length_errorPKc
.LEHE22:
.L456:
	movl	$.LC69, %edi
.LEHB23:
	call	_ZSt20__throw_length_errorPKc
.LEHE23:
.L392:
	movq	32(%rsp), %rdi
	call	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev
.L393:
	movq	40(%rsp), %rsi
	movq	%r15, %rdi
	call	_ZdlPvm
.L387:
	cmpq	%rbx, %r13
	je	.L411
.L461:
	movq	%r13, %rax
.L413:
	cmpq	$0, (%rax)
	je	.L465
.L412:
	call	_ZSt9terminatev
.L428:
.L453:
	movq	%rax, %rbp
.L414:
	movq	%rbp, %rdi
.LEHB24:
	call	_Unwind_Resume
.LEHE24:
.L429:
.L455:
	movq	%rax, %rbp
	jmp	.L387
.L386:
	movq	32(%rsp), %rdi
	call	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev
	jmp	.L387
.L465:
	addq	$8, %rax
	cmpq	%rax, %rbx
	jne	.L413
.L411:
	testq	%r13, %r13
	je	.L414
	movq	%r14, %rsi
	movq	%r13, %rdi
	subq	%r13, %rsi
	call	_ZdlPvm
	jmp	.L414
	.cfi_endproc
.LFE7543:
	.section	.gcc_except_table
.LLSDAC7543:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC7543-.LLSDACSBC7543
.LLSDACSBC7543:
	.uleb128 .LEHB22-.LCOLDB70
	.uleb128 .LEHE22-.LEHB22
	.uleb128 .L429-.LCOLDB70
	.uleb128 0
	.uleb128 .LEHB23-.LCOLDB70
	.uleb128 .LEHE23-.LEHB23
	.uleb128 .L428-.LCOLDB70
	.uleb128 0
	.uleb128 .LEHB24-.LCOLDB70
	.uleb128 .LEHE24-.LEHB24
	.uleb128 0
	.uleb128 0
.LLSDACSEC7543:
	.section	.text.unlikely
	.text
	.size	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE, .-_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
	.section	.text.unlikely
	.size	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE.cold, .-_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE.cold
.LCOLDE70:
	.text
.LHOTE70:
	.section	.text.unlikely
.LCOLDB73:
	.text
.LHOTB73:
	.p2align 4
	.type	_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb, @function
_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb:
.LFB8384:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8384
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rdi, %rax
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movl	%esi, %ebp
	movzbl	%cl, %esi
	leaq	1024(%rdi), %rcx
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	subq	$56, %rsp
	.cfi_def_cfa_offset 96
	movq	%rdx, 8(%rsp)
	.p2align 4
	.p2align 4
	.p2align 3
.L467:
	xorl	%edi, %edi
	xchgq	(%rax), %rdi
	addq	$64, %rax
	cmpq	%rcx, %rax
	jne	.L467
	leaq	8(%rsp), %rax
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movq	.LC71(%rip), %xmm0
	movq	%r12, 16(%rsp)
	movhps	.LC72(%rip), %xmm0
	movq	%rax, 24(%rsp)
	movaps	%xmm0, 32(%rsp)
.LEHB25:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE25:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L468
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L468:
	testl	%ebp, %ebp
	jle	.L472
	salq	$6, %rbp
	xorl	%eax, %eax
	leaq	(%r12,%rbp), %rcx
	.p2align 5
	.p2align 4
	.p2align 3
.L470:
	addq	$64, %rbx
	movq	-64(%rbx), %rdx
	addq	%rdx, %rax
	cmpq	%rcx, %rbx
	jne	.L470
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L472:
	.cfi_restore_state
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L473:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L471
	.section	.gcc_except_table
.LLSDA8384:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8384-.LLSDACSB8384
.LLSDACSB8384:
	.uleb128 .LEHB25-.LFB8384
	.uleb128 .LEHE25-.LEHB25
	.uleb128 .L473-.LFB8384
	.uleb128 0
.LLSDACSE8384:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8384
	.type	_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb.cold, @function
_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb.cold:
.LFSB8384:
.L471:
	.cfi_def_cfa_offset 96
	.cfi_offset 3, -40
	.cfi_offset 6, -32
	.cfi_offset 12, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB26:
	call	_Unwind_Resume
.LEHE26:
	.cfi_endproc
.LFE8384:
	.section	.gcc_except_table
.LLSDAC8384:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8384-.LLSDACSBC8384
.LLSDACSBC8384:
	.uleb128 .LEHB26-.LCOLDB73
	.uleb128 .LEHE26-.LEHB26
	.uleb128 0
	.uleb128 0
.LLSDACSEC8384:
	.section	.text.unlikely
	.text
	.size	_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb, .-_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb
	.section	.text.unlikely
	.size	_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb.cold, .-_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb.cold
.LCOLDE73:
	.text
.LHOTE73:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8860:
	.cfi_startproc
	movzbl	(%rcx), %ecx
	movq	(%rdx), %rdx
	movl	$_ZN12_GLOBAL__N_1L6a_hdisE, %edi
	movl	(%rsi), %esi
	jmp	_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb
	.cfi_endproc
.LFE8860:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E9_M_invokeERKSt9_Any_dataOiOmOb
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8874:
	.cfi_startproc
	movzbl	(%rcx), %ecx
	movq	(%rdx), %rdx
	movl	$_ZN12_GLOBAL__N_1L4a_64E, %edi
	movl	(%rsi), %esi
	jmp	_ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imb
	.cfi_endproc
.LFE8874:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
.LCOLDB76:
	.text
.LHOTB76:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8881:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8881
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rsi, %rax
	subq	$48, %rsp
	.cfi_def_cfa_offset 64
	movl	(%rax), %edi
	movq	(%rdx), %rax
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	xchgq	_ZN12_GLOBAL__N_1L8a_sharedE(%rip), %rax
	movq	.LC74(%rip), %xmm0
	movhps	.LC75(%rip), %xmm0
	leaq	8(%rsp), %rax
	leaq	16(%rsp), %rdx
	movaps	%xmm0, 32(%rsp)
	movq	$0, 24(%rsp)
	movq	%rax, 16(%rsp)
.LEHB27:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE27:
	movq	32(%rsp), %rax
	movq	%xmm0, %rbx
	testq	%rax, %rax
	je	.L484
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L484:
	movq	%rbx, %xmm0
	movq	_ZN12_GLOBAL__N_1L8a_sharedE(%rip), %rax
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
.L486:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L485
	.section	.gcc_except_table
.LLSDA8881:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8881-.LLSDACSB8881
.LLSDACSB8881:
	.uleb128 .LEHB27-.LFB8881
	.uleb128 .LEHE27-.LEHB27
	.uleb128 .L486-.LFB8881
	.uleb128 0
.LLSDACSE8881:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8881
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8881:
.L485:
	.cfi_def_cfa_offset 64
	.cfi_offset 3, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB28:
	call	_Unwind_Resume
.LEHE28:
	.cfi_endproc
.LFE8881:
	.section	.gcc_except_table
.LLSDAC8881:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8881-.LLSDACSBC8881
.LLSDACSBC8881:
	.uleb128 .LEHB28-.LCOLDB76
	.uleb128 .LEHE28-.LEHB28
	.uleb128 0
	.uleb128 0
.LLSDACSEC8881:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE76:
	.text
.LHOTE76:
	.section	.text.unlikely
.LCOLDB79:
	.text
.LHOTB79:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8888:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8888
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L6mergedE, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	.p2align 5
	.p2align 4
	.p2align 3
.L493:
	xorl	%edi, %edi
	xchgq	(%rax), %rdi
	addq	$64, %rax
	cmpq	$_ZN12_GLOBAL__N_1L6mergedE+1024, %rax
	jne	.L493
	leaq	8(%rsp), %rax
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movq	.LC77(%rip), %xmm0
	movq	$0, 24(%rsp)
	movhps	.LC78(%rip), %xmm0
	movq	%rax, 16(%rsp)
	movaps	%xmm0, 32(%rsp)
.LEHB29:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE29:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L494
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L494:
	testl	%ebp, %ebp
	jle	.L498
	salq	$6, %rbp
	xorl	%eax, %eax
	leaq	_ZN12_GLOBAL__N_1L6mergedE(%rbp), %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L496:
	addq	$64, %rbx
	movq	-64(%rbx), %rcx
	addq	%rcx, %rax
	cmpq	%rdx, %rbx
	jne	.L496
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L498:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L499:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L497
	.section	.gcc_except_table
.LLSDA8888:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8888-.LLSDACSB8888
.LLSDACSB8888:
	.uleb128 .LEHB29-.LFB8888
	.uleb128 .LEHE29-.LEHB29
	.uleb128 .L499-.LFB8888
	.uleb128 0
.LLSDACSE8888:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8888
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8888:
.L497:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB30:
	call	_Unwind_Resume
.LEHE30:
	.cfi_endproc
.LFE8888:
	.section	.gcc_except_table
.LLSDAC8888:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8888-.LLSDACSBC8888
.LLSDACSBC8888:
	.uleb128 .LEHB30-.LCOLDB79
	.uleb128 .LEHE30-.LEHB30
	.uleb128 0
	.uleb128 0
.LLSDACSEC8888:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE79:
	.text
.LHOTE79:
	.section	.text.unlikely
.LCOLDB82:
	.text
.LHOTB82:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8895:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8895
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L6mergedE, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	.p2align 5
	.p2align 4
	.p2align 3
.L508:
	xorl	%edi, %edi
	xchgq	(%rax), %rdi
	addq	$64, %rax
	cmpq	$_ZN12_GLOBAL__N_1L6mergedE+1024, %rax
	jne	.L508
	leaq	8(%rsp), %rax
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movq	.LC80(%rip), %xmm0
	movq	$0, 24(%rsp)
	movhps	.LC81(%rip), %xmm0
	movq	%rax, 16(%rsp)
	movaps	%xmm0, 32(%rsp)
.LEHB31:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE31:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L509
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L509:
	testl	%ebp, %ebp
	jle	.L513
	salq	$6, %rbp
	xorl	%eax, %eax
	leaq	_ZN12_GLOBAL__N_1L6mergedE(%rbp), %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L511:
	addq	$64, %rbx
	movq	-64(%rbx), %rcx
	addq	%rcx, %rax
	cmpq	%rdx, %rbx
	jne	.L511
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L513:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L514:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L512
	.section	.gcc_except_table
.LLSDA8895:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8895-.LLSDACSB8895
.LLSDACSB8895:
	.uleb128 .LEHB31-.LFB8895
	.uleb128 .LEHE31-.LEHB31
	.uleb128 .L514-.LFB8895
	.uleb128 0
.LLSDACSE8895:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8895
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8895:
.L512:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB32:
	call	_Unwind_Resume
.LEHE32:
	.cfi_endproc
.LFE8895:
	.section	.gcc_except_table
.LLSDAC8895:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8895-.LLSDACSBC8895
.LLSDACSBC8895:
	.uleb128 .LEHB32-.LCOLDB82
	.uleb128 .LEHE32-.LEHB32
	.uleb128 0
	.uleb128 0
.LLSDACSEC8895:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE82:
	.text
.LHOTE82:
	.section	.text.unlikely
.LCOLDB85:
	.text
.LHOTB85:
	.p2align 4
	.type	_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb, @function
_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb:
.LFB8396:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8396
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rdi, %rax
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movl	%esi, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	subq	$56, %rsp
	.cfi_def_cfa_offset 96
	movq	%rdx, 8(%rsp)
	leaq	1024(%rdi), %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L523:
	movq	$0, (%rax)
	subq	$-128, %rax
	movq	$0, -64(%rax)
	cmpq	%rdx, %rax
	jne	.L523
	leaq	8(%rsp), %rax
	movzbl	%cl, %esi
	leaq	16(%rsp), %rdx
	movl	%r12d, %edi
	movq	.LC83(%rip), %xmm0
	movq	%rbp, 16(%rsp)
	movhps	.LC84(%rip), %xmm0
	movq	%rax, 24(%rsp)
	movaps	%xmm0, 32(%rsp)
.LEHB33:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE33:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L524
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L524:
	testl	%r12d, %r12d
	jle	.L528
	movl	%r12d, %r12d
	xorl	%eax, %eax
	salq	$6, %r12
	leaq	0(%rbp,%r12), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L526:
	addq	(%rbx), %rax
	addq	$64, %rbx
	cmpq	%rdx, %rbx
	jne	.L526
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L528:
	.cfi_restore_state
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L529:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L527
	.section	.gcc_except_table
.LLSDA8396:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8396-.LLSDACSB8396
.LLSDACSB8396:
	.uleb128 .LEHB33-.LFB8396
	.uleb128 .LEHE33-.LEHB33
	.uleb128 .L529-.LFB8396
	.uleb128 0
.LLSDACSE8396:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8396
	.type	_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb.cold, @function
_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb.cold:
.LFSB8396:
.L527:
	.cfi_def_cfa_offset 96
	.cfi_offset 3, -40
	.cfi_offset 6, -32
	.cfi_offset 12, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB34:
	call	_Unwind_Resume
.LEHE34:
	.cfi_endproc
.LFE8396:
	.section	.gcc_except_table
.LLSDAC8396:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8396-.LLSDACSBC8396
.LLSDACSBC8396:
	.uleb128 .LEHB34-.LCOLDB85
	.uleb128 .LEHE34-.LEHB34
	.uleb128 0
	.uleb128 0
.LLSDACSEC8396:
	.section	.text.unlikely
	.text
	.size	_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb, .-_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb
	.section	.text.unlikely
	.size	_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb.cold, .-_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb.cold
.LCOLDE85:
	.text
.LHOTE85:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8909:
	.cfi_startproc
	movzbl	(%rcx), %ecx
	movq	(%rdx), %rdx
	movl	$_ZN12_GLOBAL__N_1L6p_hdisE, %edi
	movl	(%rsi), %esi
	jmp	_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb
	.cfi_endproc
.LFE8909:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E9_M_invokeERKSt9_Any_dataOiOmOb
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8923:
	.cfi_startproc
	movzbl	(%rcx), %ecx
	movq	(%rdx), %rdx
	movl	$_ZN12_GLOBAL__N_1L4p_64E, %edi
	movl	(%rsi), %esi
	jmp	_ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imb
	.cfi_endproc
.LFE8923:
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
.LCOLDB88:
	.text
.LHOTB88:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8930:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8930
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L8p_mergedE, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	.p2align 5
	.p2align 4
	.p2align 3
.L541:
	movq	$0, (%rax)
	subq	$-128, %rax
	movq	$0, -64(%rax)
	cmpq	$_ZN12_GLOBAL__N_1L8p_mergedE+1024, %rax
	jne	.L541
	leaq	8(%rsp), %rax
	movzbl	%sil, %esi
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movq	.LC86(%rip), %xmm0
	movq	%rax, 16(%rsp)
	movhps	.LC87(%rip), %xmm0
	movq	$0, 24(%rsp)
	movaps	%xmm0, 32(%rsp)
.LEHB35:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE35:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L542
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L542:
	testl	%ebp, %ebp
	jle	.L546
	movl	%ebp, %ebp
	xorl	%eax, %eax
	salq	$6, %rbp
	leaq	_ZN12_GLOBAL__N_1L8p_mergedE(%rbp), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L544:
	addq	(%rbx), %rax
	addq	$64, %rbx
	cmpq	%rdx, %rbx
	jne	.L544
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L546:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L547:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L545
	.section	.gcc_except_table
.LLSDA8930:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8930-.LLSDACSB8930
.LLSDACSB8930:
	.uleb128 .LEHB35-.LFB8930
	.uleb128 .LEHE35-.LEHB35
	.uleb128 .L547-.LFB8930
	.uleb128 0
.LLSDACSE8930:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8930
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8930:
.L545:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB36:
	call	_Unwind_Resume
.LEHE36:
	.cfi_endproc
.LFE8930:
	.section	.gcc_except_table
.LLSDAC8930:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8930-.LLSDACSBC8930
.LLSDACSBC8930:
	.uleb128 .LEHB36-.LCOLDB88
	.uleb128 .LEHE36-.LEHB36
	.uleb128 0
	.uleb128 0
.LLSDACSEC8930:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE88:
	.text
.LHOTE88:
	.section	.text.unlikely
.LCOLDB91:
	.text
.LHOTB91:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8937:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8937
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L8p_mergedE, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	.p2align 5
	.p2align 4
	.p2align 3
.L557:
	movq	$0, (%rax)
	subq	$-128, %rax
	movq	$0, -64(%rax)
	cmpq	$_ZN12_GLOBAL__N_1L8p_mergedE+1024, %rax
	jne	.L557
	leaq	8(%rsp), %rax
	movzbl	%sil, %esi
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movq	.LC89(%rip), %xmm0
	movq	%rax, 16(%rsp)
	movhps	.LC90(%rip), %xmm0
	movq	$0, 24(%rsp)
	movaps	%xmm0, 32(%rsp)
.LEHB37:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE37:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L558
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L558:
	testl	%ebp, %ebp
	jle	.L562
	movl	%ebp, %ebp
	xorl	%eax, %eax
	salq	$6, %rbp
	leaq	_ZN12_GLOBAL__N_1L8p_mergedE(%rbp), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L560:
	addq	(%rbx), %rax
	addq	$64, %rbx
	cmpq	%rdx, %rbx
	jne	.L560
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L562:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L563:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L561
	.section	.gcc_except_table
.LLSDA8937:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8937-.LLSDACSB8937
.LLSDACSB8937:
	.uleb128 .LEHB37-.LFB8937
	.uleb128 .LEHE37-.LEHB37
	.uleb128 .L563-.LFB8937
	.uleb128 0
.LLSDACSE8937:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8937
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8937:
.L561:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB38:
	call	_Unwind_Resume
.LEHE38:
	.cfi_endproc
.LFE8937:
	.section	.gcc_except_table
.LLSDAC8937:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8937-.LLSDACSBC8937
.LLSDACSBC8937:
	.uleb128 .LEHB38-.LCOLDB91
	.uleb128 .LEHE38-.LEHB38
	.uleb128 0
	.uleb128 0
.LLSDACSEC8937:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE91:
	.text
.LHOTE91:
	.section	.text.unlikely
.LCOLDB95:
	.text
.LHOTB95:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8916:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8916
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L5p_128E, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movq	.LC92(%rip), %xmm0
	leaq	8(%rsp), %rsi
	movq	%rsi, %xmm1
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	punpcklqdq	%xmm1, %xmm0
	.p2align 5
	.p2align 4
	.p2align 3
.L573:
	movq	$0, (%rax)
	addq	$256, %rax
	movq	$0, -128(%rax)
	cmpq	$_ZN12_GLOBAL__N_1L5p_128E+2048, %rax
	jne	.L573
	movzbl	%sil, %esi
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movaps	%xmm0, 16(%rsp)
	movq	.LC93(%rip), %xmm0
	movhps	.LC94(%rip), %xmm0
	movaps	%xmm0, 32(%rsp)
.LEHB39:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE39:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L574
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L574:
	testl	%ebp, %ebp
	jle	.L578
	movl	%ebp, %ebp
	xorl	%eax, %eax
	salq	$7, %rbp
	leaq	_ZN12_GLOBAL__N_1L5p_128E(%rbp), %rdx
	.p2align 4
	.p2align 4
	.p2align 3
.L576:
	addq	(%rbx), %rax
	subq	$-128, %rbx
	cmpq	%rdx, %rbx
	jne	.L576
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L578:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L579:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L577
	.section	.gcc_except_table
.LLSDA8916:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8916-.LLSDACSB8916
.LLSDACSB8916:
	.uleb128 .LEHB39-.LFB8916
	.uleb128 .LEHE39-.LEHB39
	.uleb128 .L579-.LFB8916
	.uleb128 0
.LLSDACSE8916:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8916
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8916:
.L577:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB40:
	call	_Unwind_Resume
.LEHE40:
	.cfi_endproc
.LFE8916:
	.section	.gcc_except_table
.LLSDAC8916:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8916-.LLSDACSBC8916
.LLSDACSBC8916:
	.uleb128 .LEHB40-.LCOLDB95
	.uleb128 .LEHE40-.LEHB40
	.uleb128 0
	.uleb128 0
.LLSDACSEC8916:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE95:
	.text
.LHOTE95:
	.section	.text.unlikely
.LCOLDB99:
	.text
.LHOTB99:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8902:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8902
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pxor	%xmm0, %xmm0
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$56, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebx
	movq	(%rdx), %rax
	movzbl	(%rcx), %esi
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE(%rip)
	leaq	16(%rsp), %rdx
	movq	%rax, 8(%rsp)
	leaq	8(%rsp), %rax
	movl	%ebx, %edi
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+16(%rip)
	movq	%rax, %xmm1
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+32(%rip)
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+48(%rip)
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+64(%rip)
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+80(%rip)
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+96(%rip)
	movaps	%xmm0, _ZN12_GLOBAL__N_1L8p_packedE+112(%rip)
	movq	.LC96(%rip), %xmm0
	punpcklqdq	%xmm1, %xmm0
	movaps	%xmm0, 16(%rsp)
	movq	.LC97(%rip), %xmm0
	movhps	.LC98(%rip), %xmm0
	movaps	%xmm0, 32(%rsp)
.LEHB41:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE41:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L589
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L589:
	testl	%ebx, %ebx
	jle	.L593
	movl	$_ZN12_GLOBAL__N_1L8p_packedE, %eax
	leaq	_ZN12_GLOBAL__N_1L8p_packedE(,%rbx,8), %rcx
	xorl	%edx, %edx
	.p2align 4
	.p2align 4
	.p2align 3
.L591:
	addq	(%rax), %rdx
	addq	$8, %rax
	cmpq	%rax, %rcx
	jne	.L591
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%r14, %xmm0
	movq	%rdx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L593:
	.cfi_restore_state
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	xorl	%edx, %edx
	movq	%r14, %xmm0
	movq	%rdx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L594:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L592
	.section	.gcc_except_table
.LLSDA8902:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8902-.LLSDACSB8902
.LLSDACSB8902:
	.uleb128 .LEHB41-.LFB8902
	.uleb128 .LEHE41-.LEHB41
	.uleb128 .L594-.LFB8902
	.uleb128 0
.LLSDACSE8902:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8902
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8902:
.L592:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB42:
	call	_Unwind_Resume
.LEHE42:
	.cfi_endproc
.LFE8902:
	.section	.gcc_except_table
.LLSDAC8902:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8902-.LLSDACSBC8902
.LLSDACSBC8902:
	.uleb128 .LEHB42-.LCOLDB99
	.uleb128 .LEHE42-.LEHB42
	.uleb128 0
	.uleb128 0
.LLSDACSEC8902:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE99:
	.text
.LHOTE99:
	.section	.text.unlikely
.LCOLDB103:
	.text
.LHOTB103:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8867:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8867
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L5a_128E, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movq	.LC100(%rip), %xmm0
	leaq	8(%rsp), %rsi
	movq	%rsi, %xmm1
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	punpcklqdq	%xmm1, %xmm0
	.p2align 5
	.p2align 4
	.p2align 3
.L602:
	xorl	%edi, %edi
	xchgq	(%rax), %rdi
	subq	$-128, %rax
	cmpq	$_ZN12_GLOBAL__N_1L5a_128E+2048, %rax
	jne	.L602
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movaps	%xmm0, 16(%rsp)
	movq	.LC101(%rip), %xmm0
	movhps	.LC102(%rip), %xmm0
	movaps	%xmm0, 32(%rsp)
.LEHB43:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE43:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L603
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L603:
	testl	%ebp, %ebp
	jle	.L607
	salq	$7, %rbp
	xorl	%eax, %eax
	leaq	_ZN12_GLOBAL__N_1L5a_128E(%rbp), %rdx
	.p2align 5
	.p2align 4
	.p2align 3
.L605:
	subq	$-128, %rbx
	movq	-128(%rbx), %rcx
	addq	%rcx, %rax
	cmpq	%rdx, %rbx
	jne	.L605
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L607:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L608:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L606
	.section	.gcc_except_table
.LLSDA8867:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8867-.LLSDACSB8867
.LLSDACSB8867:
	.uleb128 .LEHB43-.LFB8867
	.uleb128 .LEHE43-.LEHB43
	.uleb128 .L608-.LFB8867
	.uleb128 0
.LLSDACSE8867:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8867
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8867:
.L606:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB44:
	call	_Unwind_Resume
.LEHE44:
	.cfi_endproc
.LFE8867:
	.section	.gcc_except_table
.LLSDAC8867:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8867-.LLSDACSBC8867
.LLSDACSBC8867:
	.uleb128 .LEHB44-.LCOLDB103
	.uleb128 .LEHE44-.LEHB44
	.uleb128 0
	.uleb128 0
.LLSDACSEC8867:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE103:
	.text
.LHOTE103:
	.section	.text.unlikely
.LCOLDB107:
	.text
.LHOTB107:
	.p2align 4
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb:
.LFB8851:
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA8851
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rax
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movl	$_ZN12_GLOBAL__N_1L8a_packedE, %ebx
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movl	(%rax), %ebp
	movq	(%rdx), %rax
	movq	.LC104(%rip), %xmm0
	leaq	8(%rsp), %rsi
	movq	%rsi, %xmm1
	movzbl	(%rcx), %esi
	movq	%rax, 8(%rsp)
	movq	%rbx, %rax
	punpcklqdq	%xmm1, %xmm0
	.p2align 5
	.p2align 4
	.p2align 3
.L617:
	xorl	%edi, %edi
	xchgq	(%rax), %rdi
	addq	$8, %rax
	cmpq	$_ZN12_GLOBAL__N_1L8a_packedE+128, %rax
	jne	.L617
	leaq	16(%rsp), %rdx
	movl	%ebp, %edi
	movaps	%xmm0, 16(%rsp)
	movq	.LC105(%rip), %xmm0
	movhps	.LC106(%rip), %xmm0
	movaps	%xmm0, 32(%rsp)
.LEHB45:
	call	_ZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEE
.LEHE45:
	movq	32(%rsp), %rax
	movq	%xmm0, %r14
	testq	%rax, %rax
	je	.L618
	leaq	16(%rsp), %rsi
	movl	$3, %edx
	movq	%rsi, %rdi
	call	*%rax
.L618:
	testl	%ebp, %ebp
	jle	.L622
	leaq	_ZN12_GLOBAL__N_1L8a_packedE(,%rbp,8), %rcx
	xorl	%eax, %eax
	.p2align 5
	.p2align 4
	.p2align 3
.L620:
	addq	$8, %rbx
	movq	-8(%rbx), %rdx
	addq	%rdx, %rax
	cmpq	%rcx, %rbx
	jne	.L620
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L622:
	.cfi_restore_state
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	movq	%r14, %xmm0
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L623:
	.cfi_restore_state
	movq	%rax, %rbx
	jmp	.L621
	.section	.gcc_except_table
.LLSDA8851:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE8851-.LLSDACSB8851
.LLSDACSB8851:
	.uleb128 .LEHB45-.LFB8851
	.uleb128 .LEHE45-.LEHB45
	.uleb128 .L623-.LFB8851
	.uleb128 0
.LLSDACSE8851:
	.text
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDAC8851
	.type	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, @function
_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb.cold:
.LFSB8851:
.L621:
	.cfi_def_cfa_offset 80
	.cfi_offset 3, -32
	.cfi_offset 6, -24
	.cfi_offset 14, -16
	leaq	16(%rsp), %rdi
	call	_ZNSt14_Function_baseD2Ev
	movq	%rbx, %rdi
.LEHB46:
	call	_Unwind_Resume
.LEHE46:
	.cfi_endproc
.LFE8851:
	.section	.gcc_except_table
.LLSDAC8851:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC8851-.LLSDACSBC8851
.LLSDACSBC8851:
	.uleb128 .LEHB46-.LCOLDB107
	.uleb128 .LEHE46-.LEHB46
	.uleb128 0
	.uleb128 0
.LLSDACSEC8851:
	.section	.text.unlikely
	.text
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb
	.section	.text.unlikely
	.size	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb.cold, .-_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb.cold
.LCOLDE107:
	.text
.LHOTE107:
	.section	.rodata
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L10run_sharedEimbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L10run_sharedEimbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L10run_sharedEimbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L10run_sharedEimbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L10run_sharedEimbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L10run_sharedEimbEUliE_, 41
_ZTSZN12_GLOBAL__N_1L10run_sharedEimbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L10run_sharedEimbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_, 45
_ZTSZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_, 44
_ZTSZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_, 47
_ZTSZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_, 46
_ZTSZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_, 45
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE0_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE1_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE2_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE3_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE4_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE5_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE6_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE7_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE8_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_, 46
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE9_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_, 47
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE10_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_, @object
	.size	_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_, 16
_ZTIZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_, @object
	.size	_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_, 47
_ZTSZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_:
	.string	"*ZN12_GLOBAL__N_1L13make_variantsEvEUliT_bE11_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, 97
_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, 98
_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, 99
_ZTSZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS_8AtomSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_, 97
_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm8EEELm16EEEENS_6ResultERT_imbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_, 98
_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm64EEELm16EEEENS_6ResultERT_imbEUliE_"
	.align 8
	.type	_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, 16
_ZTIZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_
	.align 32
	.type	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, @object
	.size	_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_, 99
_ZTSZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_:
	.string	"*ZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS_9PlainSlotILm128EEELm16EEEENS_6ResultERT_imbEUliE_"
	.align 8
	.type	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE, @object
	.size	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE, 24
_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE:
	.quad	_ZTVN10__cxxabiv120__si_class_type_infoE+16
	.quad	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE
	.quad	_ZTINSt6thread6_StateE
	.align 32
	.type	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE, @object
	.size	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE, 107
_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE:
	.string	"*NSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE"
	.align 8
	.type	_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE, @object
	.size	_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE, 40
_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE:
	.quad	0
	.quad	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEEE
	.quad	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED1Ev
	.quad	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEED0Ev
	.quad	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJZN12_GLOBAL__N_1L6launchEibRKSt8functionIFviEEEUlvE_EEEEE6_M_runEv
	.local	_ZN12_GLOBAL__N_1L4endsE
	.comm	_ZN12_GLOBAL__N_1L4endsE,4096,256
	.local	_ZN12_GLOBAL__N_1L8p_mergedE
	.comm	_ZN12_GLOBAL__N_1L8p_mergedE,1024,64
	.local	_ZN12_GLOBAL__N_1L4p_64E
	.comm	_ZN12_GLOBAL__N_1L4p_64E,1024,64
	.local	_ZN12_GLOBAL__N_1L5p_128E
	.comm	_ZN12_GLOBAL__N_1L5p_128E,2048,128
	.local	_ZN12_GLOBAL__N_1L6p_hdisE
	.comm	_ZN12_GLOBAL__N_1L6p_hdisE,1024,64
	.local	_ZN12_GLOBAL__N_1L8p_packedE
	.comm	_ZN12_GLOBAL__N_1L8p_packedE,128,32
	.local	_ZN12_GLOBAL__N_1L6mergedE
	.comm	_ZN12_GLOBAL__N_1L6mergedE,1024,64
	.local	_ZN12_GLOBAL__N_1L8a_sharedE
	.comm	_ZN12_GLOBAL__N_1L8a_sharedE,64,64
	.local	_ZN12_GLOBAL__N_1L4a_64E
	.comm	_ZN12_GLOBAL__N_1L4a_64E,1024,64
	.local	_ZN12_GLOBAL__N_1L5a_128E
	.comm	_ZN12_GLOBAL__N_1L5a_128E,2048,128
	.local	_ZN12_GLOBAL__N_1L6a_hdisE
	.comm	_ZN12_GLOBAL__N_1L6a_hdisE,1024,64
	.local	_ZN12_GLOBAL__N_1L8a_packedE
	.comm	_ZN12_GLOBAL__N_1L8a_packedE,128,32
	.section	.tbss,"awT",@nobits
	.align 8
	.type	_ZN12_GLOBAL__N_1L8tl_plainE, @object
	.size	_ZN12_GLOBAL__N_1L8tl_plainE, 8
_ZN12_GLOBAL__N_1L8tl_plainE:
	.zero	8
	.align 8
	.type	_ZN12_GLOBAL__N_1L9tl_atomicE, @object
	.size	_ZN12_GLOBAL__N_1L9tl_atomicE, 8
_ZN12_GLOBAL__N_1L9tl_atomicE:
	.zero	8
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC33:
	.quad	.LC17
	.align 8
.LC34:
	.quad	.LC16
	.align 8
.LC35:
	.quad	.LC18
	.align 8
.LC36:
	.quad	.LC19
	.align 8
.LC37:
	.quad	.LC20
	.align 8
.LC38:
	.quad	.LC21
	.align 8
.LC39:
	.quad	.LC22
	.align 8
.LC40:
	.quad	.LC23
	.align 8
.LC41:
	.quad	.LC24
	.align 8
.LC42:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC43:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC44:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC45:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE0_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC46:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC47:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE1_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC48:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC49:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE2_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC50:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC51:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE3_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC52:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC53:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE4_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC54:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC55:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE5_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC56:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC57:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE6_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC58:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC59:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE7_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC60:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC61:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE8_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC62:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC63:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE9_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC64:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC65:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE10_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC66:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E10_M_managerERSt9_Any_dataRKS6_St18_Manager_operation
	.align 8
.LC67:
	.quad	_ZNSt17_Function_handlerIFN12_GLOBAL__N_16ResultEimbEZNS0_L13make_variantsEvEUliT_bE11_E9_M_invokeERKSt9_Any_dataOiOmOb
	.align 8
.LC71:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.align 8
.LC72:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC74:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.align 8
.LC75:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L10run_sharedEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC77:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.align 8
.LC78:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L14run_tls_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC80:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.align 8
.LC81:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_stack_atomicEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC83:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.align 8
.LC84:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm64EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC86:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.align 8
.LC87:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L13run_tls_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC89:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E10_M_managerERSt9_Any_dataRKS4_St18_Manager_operation
	.align 8
.LC90:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_stack_plainEimbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC92:
	.quad	_ZN12_GLOBAL__N_1L5p_128E
	.align 8
.LC93:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.align 8
.LC94:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC96:
	.quad	_ZN12_GLOBAL__N_1L8p_packedE
	.align 8
.LC97:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.align 8
.LC98:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L15run_plain_arrayISt5arrayINS1_9PlainSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC100:
	.quad	_ZN12_GLOBAL__N_1L5a_128E
	.align 8
.LC101:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.align 8
.LC102:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm128EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.align 8
.LC104:
	.quad	_ZN12_GLOBAL__N_1L8a_packedE
	.align 8
.LC105:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E10_M_managerERSt9_Any_dataRKSC_St18_Manager_operation
	.align 8
.LC106:
	.quad	_ZNSt17_Function_handlerIFviEZN12_GLOBAL__N_1L16run_atomic_arrayISt5arrayINS1_8AtomSlotILm8EEELm16EEEENS1_6ResultERT_imbEUliE_E9_M_invokeERKSt9_Any_dataOi
	.globl	__gxx_personality_v0
	.ident	"GCC: (GNU) 16.2.0"
	.section	.note.GNU-stack,"",@progbits
