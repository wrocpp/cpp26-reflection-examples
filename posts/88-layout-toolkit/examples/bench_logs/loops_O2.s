	.arch armv8.5-a
	.build_version macos,  27, 0
	.text
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels6soa_ifEPKyS1_PKhm
	.weak_definition __ZN7kernels6soa_ifEPKyS1_PKhm
__ZN7kernels6soa_ifEPKyS1_PKhm:
LFB3913:
	mov	x7, x0
	cbz	x3, L5
	mov	x4, 0
	mov	x0, 0
	.p2align 5,,15
L4:
	ldrb	w5, [x2, x4]
	cbz	w5, L3
	ldr	x6, [x7, x4, lsl 3]
	ldr	x5, [x1, x4, lsl 3]
	madd	x0, x6, x5, x0
L3:
	add	x4, x4, 1
	cmp	x3, x4
	bne	L4
	ret
	.p2align 2,,3
L5:
	mov	x0, 0
	ret
LFE3913:
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels8soa_maskEPKyS1_PKhm
	.weak_definition __ZN7kernels8soa_maskEPKyS1_PKhm
__ZN7kernels8soa_maskEPKyS1_PKhm:
LFB3914:
	mov	x7, x0
	cbz	x3, L14
	mov	x4, 0
	mov	x0, 0
	.p2align 5,,15
L13:
	ldr	x6, [x1, x4, lsl 3]
	ldr	x5, [x7, x4, lsl 3]
	mul	x5, x5, x6
	ldrb	w6, [x2, x4]
	add	x4, x4, 1
	madd	x0, x5, x6, x0
	cmp	x3, x4
	bne	L13
	ret
	.p2align 2,,3
L14:
	mov	x0, 0
	ret
LFE3914:
	.align	2
	.p2align 5,,15
	.globl __Z7soa_if_PKyS0_PKhm
__Z7soa_if_PKyS0_PKhm:
LFB3919:
	b	__ZN7kernels6soa_ifEPKyS1_PKhm
LFE3919:
	.align	2
	.p2align 5,,15
	.globl __Z9soa_mask_PKyS0_PKhm
__Z9soa_mask_PKyS0_PKhm:
LFB3922:
	b	__ZN7kernels8soa_maskEPKyS1_PKhm
LFE3922:
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels4k_ifI8OrderBadEEyPKT_m
	.weak_definition __ZN7kernels4k_ifI8OrderBadEEyPKT_m
__ZN7kernels4k_ifI8OrderBadEEyPKT_m:
LFB4335:
	cbz	x1, L22
	add	x1, x0, x1, lsl 5
	mov	x2, x0
	mov	x0, 0
	.p2align 5,,15
L21:
	ldrb	w3, [x2, 24]
	tbz	x3, 0, L20
	ldr	x4, [x2]
	ldr	x3, [x2, 16]
	madd	x0, x4, x3, x0
L20:
	add	x2, x2, 32
	cmp	x1, x2
	bne	L21
	ret
	.p2align 2,,3
L22:
	mov	x0, 0
	ret
LFE4335:
	.align	2
	.p2align 5,,15
	.globl __Z8aos32_ifPK8OrderBadm
__Z8aos32_ifPK8OrderBadm:
LFB3917:
	b	__ZN7kernels4k_ifI8OrderBadEEyPKT_m
LFE3917:
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels4k_ifI9OrderGoodEEyPKT_m
	.weak_definition __ZN7kernels4k_ifI9OrderGoodEEyPKT_m
__ZN7kernels4k_ifI9OrderGoodEEyPKT_m:
LFB4336:
	cbz	x1, L32
	add	x1, x1, x1, lsl 1
	mov	x2, x0
	add	x3, x0, x1, lsl 3
	mov	x0, 0
	.p2align 5,,15
L31:
	ldrb	w1, [x2, 17]
	tbz	x1, 0, L30
	ldp	x4, x1, [x2]
	madd	x0, x4, x1, x0
L30:
	add	x2, x2, 24
	cmp	x2, x3
	bne	L31
	ret
	.p2align 2,,3
L32:
	mov	x0, 0
	ret
LFE4336:
	.align	2
	.p2align 5,,15
	.globl __Z8aos24_ifPK9OrderGoodm
__Z8aos24_ifPK9OrderGoodm:
LFB3918:
	b	__ZN7kernels4k_ifI9OrderGoodEEyPKT_m
LFE3918:
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels6k_maskI8OrderBadEEyPKT_m
	.weak_definition __ZN7kernels6k_maskI8OrderBadEEyPKT_m
__ZN7kernels6k_maskI8OrderBadEEyPKT_m:
LFB4337:
	cbz	x1, L41
	add	x1, x0, x1, lsl 5
	mov	x2, x0
	mov	x0, 0
	.p2align 5,,15
L40:
	ldr	x3, [x2]
	add	x2, x2, 32
	ldr	x4, [x2, -16]
	mul	x3, x3, x4
	ldrb	w4, [x2, -8]
	neg	x4, x4
	and	x3, x3, x4
	add	x0, x0, x3
	cmp	x1, x2
	bne	L40
	ret
	.p2align 2,,3
L41:
	mov	x0, 0
	ret
LFE4337:
	.align	2
	.p2align 5,,15
	.globl __Z10aos32_maskPK8OrderBadm
__Z10aos32_maskPK8OrderBadm:
LFB3920:
	b	__ZN7kernels6k_maskI8OrderBadEEyPKT_m
LFE3920:
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels6k_maskI9OrderGoodEEyPKT_m
	.weak_definition __ZN7kernels6k_maskI9OrderGoodEEyPKT_m
__ZN7kernels6k_maskI9OrderGoodEEyPKT_m:
LFB4338:
	cbz	x1, L47
	add	x1, x1, x1, lsl 1
	mov	x2, x0
	add	x4, x0, x1, lsl 3
	mov	x0, 0
	.p2align 5,,15
L46:
	ldp	x1, x3, [x2]
	add	x2, x2, 24
	mul	x1, x1, x3
	ldrb	w3, [x2, -7]
	neg	x3, x3
	and	x1, x1, x3
	add	x0, x0, x1
	cmp	x4, x2
	bne	L46
	ret
	.p2align 2,,3
L47:
	mov	x0, 0
	ret
LFE4338:
	.align	2
	.p2align 5,,15
	.globl __Z10aos24_maskPK9OrderGoodm
__Z10aos24_maskPK9OrderGoodm:
LFB3921:
	b	__ZN7kernels6k_maskI9OrderGoodEEyPKT_m
LFE3921:
	.align	2
	.p2align 5,,15
	.globl __ZN7kernels5k_allI8OrderBadEEyPKT_m
	.weak_definition __ZN7kernels5k_allI8OrderBadEEyPKT_m
__ZN7kernels5k_allI8OrderBadEEyPKT_m:
LFB4339:
	cbz	x1, L53
	add	x1, x0, x1, lsl 5
	mov	x2, x0
	mov	x0, 0
	.p2align 5,,15
L52:
	ldrb	w3, [x2, 24]
	add	x2, x2, 32
	ldr	x5, [x2, -16]
	ldrsb	x4, [x2, -24]
	add	x0, x3, x0
	ldr	x3, [x2, -32]
	madd	x3, x3, x5, x4
	add	x0, x3, x0
	cmp	x1, x2
	bne	L52
	ret
	.p2align 2,,3
L53:
	mov	x0, 0
	ret
LFE4339:
	.align	2
	.p2align 5,,15
	.globl __Z9aos32_allPK8OrderBadm
__Z9aos32_allPK8OrderBadm:
LFB3923:
	b	__ZN7kernels5k_allI8OrderBadEEyPKT_m
LFE3923:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x3
	.ascii "zR\0"
	.uleb128 0x1
	.sleb128 -8
	.uleb128 0x1e
	.uleb128 0x1
	.byte	0x10
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB3913-.
	.set L$set$2,LFE3913-LFB3913
	.quad L$set$2
	.uleb128 0
	.align	3
LEFDE1:
LSFDE3:
	.set L$set$3,LEFDE3-LASFDE3
	.long L$set$3
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB3914-.
	.set L$set$4,LFE3914-LFB3914
	.quad L$set$4
	.uleb128 0
	.align	3
LEFDE3:
LSFDE5:
	.set L$set$5,LEFDE5-LASFDE5
	.long L$set$5
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB3919-.
	.set L$set$6,LFE3919-LFB3919
	.quad L$set$6
	.uleb128 0
	.align	3
LEFDE5:
LSFDE7:
	.set L$set$7,LEFDE7-LASFDE7
	.long L$set$7
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB3922-.
	.set L$set$8,LFE3922-LFB3922
	.quad L$set$8
	.uleb128 0
	.align	3
LEFDE7:
LSFDE9:
	.set L$set$9,LEFDE9-LASFDE9
	.long L$set$9
LASFDE9:
	.long	LASFDE9-EH_frame1
	.quad	LFB4335-.
	.set L$set$10,LFE4335-LFB4335
	.quad L$set$10
	.uleb128 0
	.align	3
LEFDE9:
LSFDE11:
	.set L$set$11,LEFDE11-LASFDE11
	.long L$set$11
LASFDE11:
	.long	LASFDE11-EH_frame1
	.quad	LFB3917-.
	.set L$set$12,LFE3917-LFB3917
	.quad L$set$12
	.uleb128 0
	.align	3
LEFDE11:
LSFDE13:
	.set L$set$13,LEFDE13-LASFDE13
	.long L$set$13
LASFDE13:
	.long	LASFDE13-EH_frame1
	.quad	LFB4336-.
	.set L$set$14,LFE4336-LFB4336
	.quad L$set$14
	.uleb128 0
	.align	3
LEFDE13:
LSFDE15:
	.set L$set$15,LEFDE15-LASFDE15
	.long L$set$15
LASFDE15:
	.long	LASFDE15-EH_frame1
	.quad	LFB3918-.
	.set L$set$16,LFE3918-LFB3918
	.quad L$set$16
	.uleb128 0
	.align	3
LEFDE15:
LSFDE17:
	.set L$set$17,LEFDE17-LASFDE17
	.long L$set$17
LASFDE17:
	.long	LASFDE17-EH_frame1
	.quad	LFB4337-.
	.set L$set$18,LFE4337-LFB4337
	.quad L$set$18
	.uleb128 0
	.align	3
LEFDE17:
LSFDE19:
	.set L$set$19,LEFDE19-LASFDE19
	.long L$set$19
LASFDE19:
	.long	LASFDE19-EH_frame1
	.quad	LFB3920-.
	.set L$set$20,LFE3920-LFB3920
	.quad L$set$20
	.uleb128 0
	.align	3
LEFDE19:
LSFDE21:
	.set L$set$21,LEFDE21-LASFDE21
	.long L$set$21
LASFDE21:
	.long	LASFDE21-EH_frame1
	.quad	LFB4338-.
	.set L$set$22,LFE4338-LFB4338
	.quad L$set$22
	.uleb128 0
	.align	3
LEFDE21:
LSFDE23:
	.set L$set$23,LEFDE23-LASFDE23
	.long L$set$23
LASFDE23:
	.long	LASFDE23-EH_frame1
	.quad	LFB3921-.
	.set L$set$24,LFE3921-LFB3921
	.quad L$set$24
	.uleb128 0
	.align	3
LEFDE23:
LSFDE25:
	.set L$set$25,LEFDE25-LASFDE25
	.long L$set$25
LASFDE25:
	.long	LASFDE25-EH_frame1
	.quad	LFB4339-.
	.set L$set$26,LFE4339-LFB4339
	.quad L$set$26
	.uleb128 0
	.align	3
LEFDE25:
LSFDE27:
	.set L$set$27,LEFDE27-LASFDE27
	.long L$set$27
LASFDE27:
	.long	LASFDE27-EH_frame1
	.quad	LFB3923-.
	.set L$set$28,LFE3923-LFB3923
	.quad L$set$28
	.uleb128 0
	.align	3
LEFDE27:
	.ident	"GCC: (Homebrew GCC 16.2.0) 16.2.0"
	.subsections_via_symbols
