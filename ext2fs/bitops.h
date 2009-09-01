/*
 * bitops.h --- Bitmap frobbing code.
 * 
 * Copyright (C) 1993, 1994 Theodore Ts'o.  This file may be
 * redistributed under the terms of the GNU Public License.
 * 
 * Taken from <asm/bitops.h>, Copyright 1992, Linus Torvalds.
 */


extern int set_bit(int nr,void * addr);
extern int clear_bit(int nr, void * addr);
extern int test_bit(int nr, const void * addr);

/*
 * EXT2FS bitmap manipulation routines.
 */

/* Support for sending warning messages from the inline subroutines */
extern const char *ext2fs_block_string;
extern const char *ext2fs_inode_string;
extern const char *ext2fs_mark_string;
extern const char *ext2fs_unmark_string;
extern const char *ext2fs_test_string;
extern void ext2fs_warn_bitmap(errcode_t errcode, unsigned long arg,
			       const char *description);

extern void ext2fs_mark_block_bitmap(ext2fs_block_bitmap bitmap, blk_t block);
extern void ext2fs_unmark_block_bitmap(ext2fs_block_bitmap bitmap,
				       blk_t block);
extern int ext2fs_test_block_bitmap(ext2fs_block_bitmap bitmap, blk_t block);

extern void ext2fs_mark_inode_bitmap(ext2fs_inode_bitmap bitmap, ino_t inode);
extern void ext2fs_unmark_inode_bitmap(ext2fs_inode_bitmap bitmap,
				       ino_t inode);
extern int ext2fs_test_inode_bitmap(ext2fs_inode_bitmap bitmap, ino_t inode);

/*
 * The inline routines themselves...
 * 
 * If NO_INLINE_FUNCS is defined, then we won't try to do inline
 * functions at all!
 */
#if (defined(INCLUDE_INLINE_FUNCS) || !defined(NO_INLINE_FUNCS))
#ifdef INCLUDE_INLINE_FUNCS
#define _INLINE_ extern
#else
#define _INLINE_ extern __inline__
#endif

#if (defined(__i386__) || defined(__i486__) || defined(__i586__))

#define _EXT2_HAVE_ASM_BITOPS_
	
/*
 * These are done by inline assembly for speed reasons.....
 *
 * All bitoperations return 0 if the bit was cleared before the
 * operation and != 0 if it was not.  Bit 0 is the LSB of addr; bit 32
 * is the LSB of (addr+1).
 */

/*
 * Some hacks to defeat gcc over-optimizations..
 */
struct __dummy_h { unsigned long a[100]; };
#define ADDR (*(struct __dummy_h *) addr)
#define CONST_ADDR (*(const struct __dummy_h *) addr)	

_INLINE_ int set_bit(int nr, void * addr)
{
	int oldbit;

	__asm__ __volatile__("btsl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"r" (nr));
	return oldbit;
}

_INLINE_ int clear_bit(int nr, void * addr)
{
	int oldbit;

	__asm__ __volatile__("btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"r" (nr));
	return oldbit;
}

_INLINE_ int test_bit(int nr, const void * addr)
{
	int oldbit;

	__asm__ __volatile__("btl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit)
		:"m" (CONST_ADDR),"r" (nr));
	return oldbit;
}

#undef ADDR

#endif	/* i386 */

#ifdef __mc68000__

#define _EXT2_HAVE_ASM_BITOPS_

_INLINE_ int set_bit(int nr,void * addr)
{
	char retval;

	__asm__ __volatile__ ("bfset %2@{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr), "a" (addr));

	return retval;
}

_INLINE_ int clear_bit(int nr, void * addr)
{
	char retval;

	__asm__ __volatile__ ("bfclr %2@{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr), "a" (addr));

	return retval;
}

_INLINE_ int test_bit(int nr, const void * addr)
{
	char retval;

	__asm__ __volatile__ ("bftst %2@{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr), "a" (addr));

	return retval;
}

#endif /* __mc68000__ */

_INLINE_ void ext2fs_mark_block_bitmap(ext2fs_block_bitmap bitmap,
				       blk_t block)
{
	if ((block < bitmap->start) || (block > bitmap->end))
		ext2fs_warn_bitmap(EXT2_ET_BAD_BLOCK_MARK, block,
				   bitmap->description);
	set_bit(block - bitmap->start, bitmap->bitmap);
}

_INLINE_ void ext2fs_unmark_block_bitmap(ext2fs_block_bitmap bitmap,
					 blk_t block)
{
	if ((block < bitmap->start) || (block > bitmap->end))
		ext2fs_warn_bitmap(EXT2_ET_BAD_BLOCK_UNMARK,
				   block, bitmap->description);
	clear_bit(block - bitmap->start, bitmap->bitmap);
}

_INLINE_ int ext2fs_test_block_bitmap(ext2fs_block_bitmap bitmap,
				       blk_t block)
{
	if ((block < bitmap->start) || (block > bitmap->end))
		ext2fs_warn_bitmap(EXT2_ET_BAD_BLOCK_TEST,
				   block, bitmap->description);
	return test_bit(block - bitmap->start, bitmap->bitmap);
}

_INLINE_ void ext2fs_mark_inode_bitmap(ext2fs_inode_bitmap bitmap,
				       ino_t inode)
{
	if ((inode < bitmap->start) || (inode > bitmap->end))
		ext2fs_warn_bitmap(EXT2_ET_BAD_INODE_MARK,
				   inode, bitmap->description);
	set_bit(inode - bitmap->start, bitmap->bitmap);
}

_INLINE_ void ext2fs_unmark_inode_bitmap(ext2fs_inode_bitmap bitmap,
					 ino_t inode)
{
	if ((inode < bitmap->start) || (inode > bitmap->end))
		ext2fs_warn_bitmap(EXT2_ET_BAD_INODE_UNMARK,
				   inode, bitmap->description);
	clear_bit(inode - bitmap->start, bitmap->bitmap);
}

_INLINE_ int ext2fs_test_inode_bitmap(ext2fs_inode_bitmap bitmap,
				       ino_t inode)
{
	if ((inode < bitmap->start) || (inode > bitmap->end))
		ext2fs_warn_bitmap(EXT2_ET_BAD_INODE_TEST,
				   inode, bitmap->description);
	return test_bit(inode - bitmap->start, bitmap->bitmap);
}

#undef _INLINE_
#endif
