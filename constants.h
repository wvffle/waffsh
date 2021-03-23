#pragma once

#define PROMPT "> "

#define READER_LINE_BUFFER_SIZE 128

// NOTE: The \n is within the delimeters because
//       execvp was failing due to execution of
//       arguments like 'firefox\n'
#define PARSER_TOKEN_DELIMETERS "\n\t\r "
#define PARSER_TOKEN_BUFFER_SIZE 8
