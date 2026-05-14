CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = firewallproject-core
SRCS = main.c engine.c ip_rules.c domain_rules.c url_parser.c port_check.c nftables_adapter.c logger.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) --test --ip 192.168.1.10 --url https://example.com/login --port 443

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean run
