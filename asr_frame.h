/*
 * #include <inttypes.h>
 *  #include <netinet/in.h>
 */
struct frame {
  int32_t type;  // 102 流式识别(叠加)
  int32_t pad;
  int32_t size;
  char data[];
} __attribute__((packed));

uint32_t frame_type(const char *frame_data);

int frame_size(const char *frame_data);

const char * frame_data(const char *frame_data);
