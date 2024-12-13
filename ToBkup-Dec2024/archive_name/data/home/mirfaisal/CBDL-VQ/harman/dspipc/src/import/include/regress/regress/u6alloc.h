#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

#include <regress/pagesize.h>

/*
*
*   This is the txt version of the file
*   http:/*www.tuhs.org/Archive/Caldera-license.pdf. */
*
*   The algorithm of u6malloc/u6free was based on code from UNIX V6.
*     _________________________________________________________________
*
*   CALDERA
*
*   240 West Center Street
*   Orem, Utah 84057
*   801-765-4999 Fax 801-765-4481
*
*   January 23, 2002
*
*   Dear UNIX enthusiasts,
*
*   Caldera  International,  Inc.  hereby  grants  a fee free license that
*   includes the rights use, modify and distribute this named
*   source  code,  including creating derived binary products created from
*   the source code. The source code for which Caldera
*   International,  Inc.  grants  rights are limited to the following UNIX
*   Operating Systems that operate on the 16-Bit PDP-11
*   CPU  and  early  versions  of  the  32-Bit UNIX Operating System, with
*   specific exclusion of UNIX System III and UNIX
*    System V and successor operating systems:
*
*   32-bit 32V UNIX
*   16 bit UNIX Versions 1, 2, 3, 4, 5, 6, 7
*
*   Caldera  International,  Inc.  makes no guarantees or commitments that
*   any source code is available from Caldera
*   International, Inc.
*
*   The  following  copyright  notice applies to the source code files for
*   which this license is granted.
*
*   Copyright(C)   Caldera   International   Inc.  2001-2002.  All  rights
*   reserved.
*
*   Redistribution  and  use  in  source and binary forms, with or without
*   modification, are permitted provided that the
*
*   following conditions are met:
*
*   Redistributions of source code and documentation must retain the above
*   copyright notice, this list of conditions and the
*   following  disclaimer.  Redistributions  in binary form must reproduce
*   the above copyright notice, this list of conditions
*   and  the  following  disclaimer  in  the  documentation  and/or  other
*   materials provided with the distribution.
*
*   All  advertising materials mentioning features or use of this software
*   must display the following acknowledgement:
*
*   This   product   includes  software  developed  or  owned  by  Caldera
*   International, Inc.
*
*   Neither the name of Caldera International, Inc. nor the names of other
*   contributors may be used to endorse or promote
*
*   products  derived  from  this  software without specific prior written
*   permission.
*
*   USE  OF  THE  SOFTWARE  PROVIDED  FOR  UNDER  THIS  LICENSE BY CALDERA
*   INTERNATIONAL, INC.
*   AND  CONTRIBUTORS  ``AS  IS''  AND  ANY EXPRESS OR IMPLIED WARRANTIES,
*   INCLUDING, BUT NOT
*   LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR
*   PURPOSE  ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC.
*   BE LIABLE FOR
*   ANY  DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*   DAMAGES
*   (INCLUDING,  BUT  NOT  LIMITED  TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*   SERVICES; LOSS OF
*   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
*   ANY THEORY OF
*   LIABILITY,  WHETHER  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*   NEGLIGENCE OR
*   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*   ADVISED OF THE
*
*   POSSIBILITY OF SUCH DAMAGE.
*
*   Very truly yours,
*
*   /signed/ Bill Broderick
*
*   Bill Broderick
*
*   Director, Licensing Services
*
*   * UNIX is a registered trademark of The Open Group in the US and other
*   countries.
*
*
*     _________________________________________________________________
*
*/

/*--------------------------------------------------------------------------*
 *									MACROS	 								*
 *--------------------------------------------------------------------------*/
#define IROUND(x) ((unsigned)((((unsigned)(x)) + 4 - 1) & ~(4 - 1)))
#define PROUND(x) (((unsigned)(x) + 4096 - 1) & ~(4096 - 1))
#define FLSZ (4 * 4096)


/*--------------------------------------------------------------------------*
 *									TYPEDEFS 								*
 *--------------------------------------------------------------------------*/
/*
 * The basic "byte" type.
 */
typedef unsigned char chunk_t;

/*
 * This type will used to define a chunk of free space.  A cell will consist
 * of a non-zero count and a base address of the free memory with that many
 * continuous bytes.
 */
typedef struct {
        chunk_t* c_addr; /* a pointer to the chunk */
        size_t c_size;   /* a count of the number of bytes in the chunk */
} cell_t;

/*
 * This type will define a list of cells.  The addresses of the chunks will
 * be in increasing order.  The list will be terminated with the first zero
 * byte count.
 *
 */
typedef struct {
        cell_t* m_first; /* pointer to the first entry in list */
        cell_t* m_last;  /* pointer to the last  entry in list */
} map_t;

/*
 * The "actual" list is the traditional malloc() style free list.
 * The "master" list is a list of all the memory managed by this
 * interface -- free and in use.
 */
typedef struct {
        map_t actual;
        map_t master;
} u6p_alloc_t;

extern size_t getcore(u6p_alloc_t*, int, size_t, chunk_t**);

/*
 * Add the given cell into free list.
 */
static int u6free(map_t* map, int coalesce, cell_t cell)
{
        cell_t *pc, temp;

        /* there must be room to grow the free list */
        assert((map->m_first) ? (((unsigned)map->m_last - (unsigned)map->m_first + sizeof *map) == FLSZ) : ((map->m_first == NULL) && (map->m_last == NULL)));

        /* verify that there is a list */
        if ((pc = map->m_first)) {
                /* search free list for correct spot in insert in cell */
                for (; pc->c_addr <= cell.c_addr && pc->c_size != 0; pc++)
                        ;
        }

        /* if there are no empty spots in the free list, then problems */
        if ((map->m_last == pc) || ((map->m_last - 1)->c_size != 0)) {
                return ENOMEM;
        }

        /* check if this is a NOP */
        if ((cell.c_addr == (chunk_t*)map) || (cell.c_size == 0))
                return EOK;

        /* check if we can combine new chunk with previous chunk */
        if (coalesce && (pc > map->m_first) && (&((pc - 1)->c_addr[(pc - 1)->c_size]) == cell.c_addr)) {
                /* so far, the number of entries in the free list has not changed */
                (pc - 1)->c_size += cell.c_size;

                /* check if "combined" chunk can be combined with next chunk */
                if (&cell.c_addr[cell.c_size] == pc->c_addr) {
                        (pc - 1)->c_size += pc->c_size;

                        /* the free list will now shrink by one entry */
                        while (pc->c_size) {
                                pc++;
                                *(pc - 1) = *pc;
                        }
                }

                return EOK;
        }

        /* check if chunk can be combined with the next chunk */
        if (coalesce && (&cell.c_addr[cell.c_size] == pc->c_addr) && pc->c_size) {
                /* the number of entries in the free list has not changed */
                pc->c_addr = cell.c_addr;
                pc->c_size += cell.c_size;
                return EOK;
        }

        /* darn it, the free list will have to grow */
        assert((map->m_last - 1)->c_size == 0);
        do {
                temp = *pc;
                *pc++ = cell;
                cell = temp;
        } while (cell.c_size);

        return EOK;
}

/*
 * Allocate the given "size" bytes from the given free list.  The base
 * address of the memory will be returned.
 */
static void* u6malloc(map_t* map, size_t size, u6p_alloc_t* handle,
    size_t (*alloc)(u6p_alloc_t*, int, size_t size, chunk_t** addr))
{
        chunk_t* addr;
        cell_t *pc, cell;
        size_t nsize;
        int status;

        assert((map->m_first) ? (((unsigned)map->m_last - (unsigned)map->m_first + sizeof *map) == FLSZ) : ((map->m_first == NULL) && (map->m_last == NULL)));

        /* If the free list has never been used then allocate a free list */
        if ((map->m_first == NULL) && alloc && ((map->m_first = mmap(0, FLSZ, PROT_READ | PROT_WRITE, MAP_PRIVATEANON, NOFD, 0)) != MAP_FAILED)) {
                map->m_last = (cell_t*)((unsigned)map->m_first - sizeof *pc + FLSZ);
        }

        /* verify that there is a list */
        if ((pc = map->m_first)) {
                /* search free list for the first free chunk of the correct size */
                for (; pc->c_size; pc++) {
                        if (pc->c_size >= size) {
                                addr = pc->c_addr;
                                pc->c_addr += size;
                                if ((pc->c_size -= size) == 0) {
                                        /* we have removed an element from the list */
                                        do {
                                                pc++;
                                                (pc - 1)->c_addr = pc->c_addr;
                                        } while (((pc - 1)->c_size = pc->c_size));
                                }

                                /* return successfully */
                                return (addr);
                        }
                }
        }

        /* if there are no empty spots in the free list, return failure */
        if ((map->m_last == pc) || ((map->m_last - 1)->c_size != 0))
                return NULL;

        /* if size is zero, return a pointer to a chunk of zero size */
        if (size == 0)
                return map;

        /* cannot find the required chunk in the free list */
        if (alloc && (nsize = alloc(handle, 0, size, &addr))) {
                cell.c_addr = &addr[size];
                cell.c_size = nsize - size;
                status = u6free(map, 1, cell);
                assert(status == EOK);
                return (addr);
        }

        return NULL;
}

#if 0
/*
 * The routine use to get memory from the system.  I assumed this
 * routine will be provide by the specific test you are trying to run.
 *
 * Note that this is where the memory gets added to the master list.
 */
static size_t getcore(u6p_alloc_t *ph, int ofb, size_t size, chunk_t **addr)
{
	cell_t temp;

	temp.c_addr = *addr = mmap(NULL, temp.c_size = PROUND(size),
		PROT_READ|PROT_WRITE, MAP_ANON, NOFD, 0);

	if ((*addr != MAP_FAILED) && !ofb && ph
		&& (u6free(&ph->master, 0, temp) != EOK)) {
		munmap(*addr, temp.c_size);
		*addr = MAP_FAILED;
	}

	return (*addr != MAP_FAILED) ? temp.c_size : 0;
}
#endif

int u6p_mem_init(u6p_alloc_t** ph)
{
        u6p_alloc_t temp;

        assert(PAGESIZE == 4096);
        memset(&temp, 0, sizeof temp);

        if (u6malloc(&temp.master, 0, NULL, getcore) != &temp.master) {
                return ENOMEM;
        }

        if (u6malloc(&temp.actual, 0, NULL, getcore) != &temp.actual) {
                munmap(temp.master.m_first, FLSZ);
                return ENOMEM;
        }

        *ph = u6malloc(&temp.actual, IROUND(sizeof(u6p_alloc_t)), &temp, getcore);
        assert(*ph != NULL);
        **ph = temp;

        return EOK;
}

int u6p_mem_destroy(u6p_alloc_t** ph)
{
        int rc;
        cell_t *pc, temp = { NULL, 0 };
        map_t master, actual;

        if (ph == NULL) {
                return EINVAL;
        }

        /* make a copy first, we will be unallocating the handle */
        master = (*ph)->master;
        actual = (*ph)->actual;

        /* attempt to prevent double destroys */
        if ((actual.m_first == NULL) || (master.m_first == NULL)) {
                return EINVAL;
        }

        /* let the internal routines verify adt is sane */
        if ((rc = u6free(&actual, 1, temp)) || (rc = u6free(&master, 1, temp))) {
                return rc;
        }

        /* free memory */
        for (pc = master.m_first; pc->c_size != 0; pc++) {
                memset(pc->c_addr, 0, pc->c_size);
                if (munmap(pc->c_addr, pc->c_size) == -1) {
                        return errno;
                }
        }

        /* free free list */
        if ((munmap(master.m_first, FLSZ) == -1) || (munmap(actual.m_first, FLSZ) == -1)) {
                return errno;
        }

        return EOK;
}

void* u6p_mem_alloc(u6p_alloc_t** ph, size_t size)
{
        return u6malloc(&(*ph)->actual, IROUND(size), *ph, getcore);
}

int u6p_mem_free(u6p_alloc_t** ph, void* addr, size_t size)
{
        cell_t temp;

        temp.c_addr = addr;
        temp.c_size = IROUND(size);

        return u6free(&(*ph)->actual, 1, temp);
}
