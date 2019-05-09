#include <inttypes.h>
#include <netinet/in.h>

#include <asr_frame.h>

uint32_t frame_type(const char *frame_data)
{
  return ntohl(((struct frame *)frame_data) -> type);
}

int frame_size(const char *frame_data)
{
  return ntohl(((struct frame *)frame_data) -> size);
}

const char * frame_data(const char *frame_data)
{
  return ((struct frame *)frame_data) -> data;
}
