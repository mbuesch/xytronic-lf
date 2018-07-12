#ifndef FAKE_WDT_H_
#define FAKE_WDT_H_

#define wdt_enable(x)		do { } while (0)
#define wdt_reset()		do { } while (0)

#endif /* FAKE_WDT_H_ */
