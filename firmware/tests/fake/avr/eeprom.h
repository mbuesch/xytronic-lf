#ifndef FAKE_EEPROM_H_
#define FAKE_EEPROM_H_


#define EEMEM			__attribute__((section("eeprom")))

#define eeprom_is_ready()	1
#define eeprom_busy_wait()	do { } while (!eeprom_is_ready())

static inline uint8_t eeprom_read_byte (const uint8_t *p)
{
	return 0u;
}

static inline uint16_t eeprom_read_word (const uint16_t *p)
{
	return 0u;
}

static inline uint32_t eeprom_read_dword (const uint32_t *p)
{
	return 0u;
}

static inline float eeprom_read_float (const float *p)
{
	return 0.0f;
}

static inline void eeprom_read_block (void *d, const void *s, size_t n)
{
}

static inline void eeprom_write_byte (uint8_t *p, uint8_t v)
{
}

static inline void eeprom_write_word (uint16_t *p, uint16_t v)
{
}

static inline void eeprom_write_dword (uint32_t *p, uint32_t v)
{
}

static inline void eeprom_write_float (float *p, float v)
{
}

static inline void eeprom_write_block (const void *s, void *d, size_t n)
{
}

static inline void eeprom_update_byte (uint8_t *p, uint8_t v)
{
}

static inline void eeprom_update_word (uint16_t *p, uint16_t v)
{
}

static inline void eeprom_update_dword (uint32_t *p, uint32_t v)
{
}

static inline void eeprom_update_float (float *p, float v)
{
}

static inline void eeprom_update_block (const void *s, void *d, size_t n)
{
}


#endif /* FAKE_EEPROM_H_ */
