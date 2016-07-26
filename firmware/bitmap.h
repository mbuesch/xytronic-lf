#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>
#include <string.h>


typedef uint8_t bm_base_type_t;

/* Define a large bit map. */
#define DEFINE_BITMAP(name, nr_bits)	bm_base_type_t name[(nr_bits) / 8]


/* Efficiently convert a bit number to the corresponding bit map.
 * Resolves at compile-time, if bitnr is a compile-time constant.
 * Uses an efficient table lookup otherwise.
 */
extern const uint8_t _bit_to_mask8[8];
#define BITMASK8(bitnr)		(			\
		(uint8_t)(__builtin_constant_p(bitnr) ?	\
			  (1u << (bitnr)) :		\
			  _bit_to_mask8[(bitnr)])	\
	)

/* Get a bit from a large bit map.
 * Return: The return value is either 0 or non-zero depending on whether
 *         the bit is 0 or 1.
 */
static inline uint8_t bm_get_bit(const bm_base_type_t *bm, uint8_t bit_number)
{
	uint8_t byte_number = bit_number >> 3;
	bit_number = bit_number & 7u;
	return (uint8_t)(bm[byte_number] & BITMASK8(bit_number));
}

/* Get a bit from a large bit map.
 * Return: The return value is either 0 or 1 depending on whether
 *         the bit is 0 or 1.
 */
static inline uint8_t bm_get_bool(const bm_base_type_t *bm, uint8_t bit_number)
{
	return !!bm_get_bit(bm, bit_number);
}

/* Set a bit in a large bit map. */
static inline void bm_set_bit(bm_base_type_t *bm, uint8_t bit_number)
{
	uint8_t byte_number = bit_number >> 3;
	bit_number = bit_number & 7u;
	bm[byte_number] = (bm_base_type_t)(bm[byte_number] | BITMASK8(bit_number));
}

/* Clear a bit in a large bit map. */
static inline void bm_clear_bit(bm_base_type_t *bm, uint8_t bit_number)
{
	uint8_t byte_number = bit_number >> 3;
	bit_number = bit_number & 7u;
	bm[byte_number] = (bm_base_type_t)(bm[byte_number] & ~BITMASK8(bit_number));
}

/* Write a bit in a large bit map. */
static inline void bm_write_bit(bm_base_type_t *bm,
				uint8_t bit_number, uint8_t bit_value)
{
	if (bit_value)
		bm_set_bit(bm, bit_number);
	else
		bm_clear_bit(bm, bit_number);
}

/* Initialize all bits in the bit map. */
#define bm_init(bm, all_set)	_bm_init(bm, sizeof(bm), all_set)

static inline void _bm_init(bm_base_type_t *bm, uint8_t bm_size,
			    uint8_t all_set)
{
	memset(bm, all_set ? 0xFF : 0, bm_size);
}

#endif /* BITMAP_H_ */
