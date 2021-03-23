#pragma once

#define PROMPT "> "

#define READER_LINE_BUFFER_SIZE 256
#define READER_FILE_BUFFER_SIZE 1024

// NOTE: The \n is within the delimeters because
//       execvp was failing due to execution of
//       arguments like 'firefox\n'.
//
//       '#' and '&' are here as they're
//       acting as special characters that do magic.
//       '>' and '|' is handled separately.
#define PARSER_TOKEN_DELIMETERS "\n\t\r\v\f #&"
#define PARSER_TOKEN_BUFFER_SIZE 8
