#ifndef _UARM_PROTOCOL_H_
#define _UARM_PROTOCOL_H_

#include "uarm_common.h"

#define TAIL_BUFFER_SIZE 		(50)
#define EVENT_BUFFER_SIZE		(50)
#define CMD_BUFFER_SIZE 		(50)
#define PACK_MAX_SIZE				(20)

enum uarm_protocol_e {
	UARM_CMD_OK = 0,
	UARM_CMD_NOTFIND,
	UARM_SYS_ABORT,
	
	UARM_CMD_ERROR = 50,
	UARM_COORD_ERROR,
};

struct protocol_private_t {
};

extern char event_report_str[EVENT_BUFFER_SIZE];

enum uarm_protocol_e uarm_execute_g_cmd(uint16_t cmd, char *line, uint8_t *char_counter);
enum uarm_protocol_e uarm_execute_p_cmd(uint16_t cmd, char *line, uint8_t *char_counter);
enum uarm_protocol_e uarm_execute_m_cmd(uint16_t cmd, char *line, uint8_t *char_counter);

void receive_cmd_line(char *line);
void parse_cmd_line(void);
void report_parse_result(void);



#endif

