#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsers.h"
#include <unistd.h>
#include "variables.h"
#include "debug.h"

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,            // |
    TOKEN_BACKGROUND,      // &
    TOKEN_SEMICOLON,       // ;
    TOKEN_REDIRECT_OUT,    // >
    TOKEN_REDIRECT_APPEND, // >>
    TOKEN_REDIRECT_IN,     // <
    TOKEN_REDIRECT_ERR,    // 2>
    TOKEN_REDIRECT_BOTH,   // &>
    TOKEN_AND,             // &&  NEW
    TOKEN_OR,              // ||  NEW
    TOKEN_EOF
} token_type_t;

typedef struct
{
    token_type_t type;
    char *value;
} token_t;

// Enhanced tokenizer with redirection support
static token_t *tokenize(char *input, int *token_count)
{
    static token_t tokens[256];
    *token_count = 0;

    char *pos = input;

    while (*pos && *token_count < 255)
    {
        // Skip whitespace
        while (isspace(*pos))
            pos++;
        if (!*pos)
            break;

        token_t *token = &tokens[*token_count];

        // Check for redirection operators (order matters! very important )
        if (*pos == '&' && *(pos + 1) == '&')
        {
            token->type = TOKEN_AND;
            token->value = strdup("&&");
            pos += 2;
        }
        else if (*pos == '|' && *(pos + 1) == '|')
        {
            token->type = TOKEN_OR;
            token->value = strdup("||");
            pos += 2;
        }
        else if (*pos == '>' && *(pos + 1) == '>')
        {
            token->type = TOKEN_REDIRECT_APPEND;
            token->value = strdup(">>");
            pos += 2;
        }
        else if (*pos == '&' && *(pos + 1) == '>')
        {
            token->type = TOKEN_REDIRECT_BOTH;
            token->value = strdup("&>");
            pos += 2;
        }
        else if (*pos == '2' && *(pos + 1) == '>')
        {
            token->type = TOKEN_REDIRECT_ERR;
            token->value = strdup("2>");
            pos += 2;
        }
        else if (*pos == '>')
        {
            token->type = TOKEN_REDIRECT_OUT;
            token->value = strdup(">");
            pos++;
        }
        else if (*pos == '<')
        {
            token->type = TOKEN_REDIRECT_IN;
            token->value = strdup("<");
            pos++;
        }
        else if (*pos == '|')
        {
            token->type = TOKEN_PIPE;
            token->value = strdup("|");
            pos++;
        }
        else if (*pos == '&')
        {
            token->type = TOKEN_BACKGROUND;
            token->value = strdup("&");
            pos++;
        }
        else if (*pos == ';')
        {
            token->type = TOKEN_SEMICOLON;
            token->value = strdup(";");
            pos++;
        }
        else if (*pos == '"' || *pos == '\'')
        {
            // Handle quoted strings (was having issue due to this)
            char quote_char = *pos;
            pos++;
            char *start = pos;

            while (*pos && *pos != quote_char)
                pos++;

            if (*pos == quote_char)
            {
                int len = pos - start;
                token->type = TOKEN_WORD;
                token->value = malloc(len + 1);
                strncpy(token->value, start, len);
                token->value[len] = '\0';
                pos++;
            }
            else
            {
                // Handle unclosed quote as regular word
                pos = start - 1;
                char *word_start = pos;
                while (*pos && !isspace(*pos) && *pos != '&' && *pos != ';' &&
                       *pos != '|' && *pos != '>' && *pos != '<')
                {
                    pos++;
                }
                int len = pos - word_start;
                token->type = TOKEN_WORD;
                token->value = malloc(len + 1);
                strncpy(token->value, word_start, len);
                token->value[len] = '\0';
            }
        }
        else
        {
            // Regular word token
            token->type = TOKEN_WORD;
            char *start = pos;
            while (*pos && !isspace(*pos) && *pos != '&' && *pos != ';' &&
                   *pos != '|' && *pos != '>' && *pos != '<')
            {
                pos++;
            }

            int len = pos - start;
            token->value = malloc(len + 1);
            strncpy(token->value, start, len);
            token->value[len] = '\0';
        }

        (*token_count)++;
    }

    // EOF token
    tokens[*token_count].type = TOKEN_EOF;
    tokens[*token_count].value = NULL;
    (*token_count)++;

    return tokens;
}

void parse_command_line(char *input, command_list_t *cmd_list)
{
    cmd_list->count = 0;
    int token_count;

    char *expanded_input = expand_variables(input);
    token_t *tokens = tokenize(expanded_input, &token_count);

    DEBUG_VERBOSE("Tokens after expansion:");
    for (int i = 0; i < token_count && tokens[i].type != TOKEN_EOF; i++)
    {
        DEBUG_VERBOSE("Token %d: '%s'", i, tokens[i].value);
    }

    int token_idx = 0;

    while (tokens[token_idx].type != TOKEN_EOF && cmd_list->count < MAX_COMMANDS)
    {
        command_t *current_cmd = &cmd_list->commands[cmd_list->count];
        current_cmd->is_background = 0;
        current_cmd->redirect_count = 0;
        current_cmd->condition = EXEC_NORMAL;
        int arg_count = 0;

        // Parse command and arguments
        while (tokens[token_idx].type == TOKEN_WORD && arg_count < MAX_ARGS - 1)
        {
            current_cmd->args[arg_count++] = strdup(tokens[token_idx].value);
            token_idx++;
        }
        current_cmd->args[arg_count] = NULL;

        // Parse redirections (your existing redirection code)
        while (token_idx < token_count &&
               current_cmd->redirect_count < MAX_REDIRECTIONS)
        {

            redirection_t *redir = &current_cmd->redirections[current_cmd->redirect_count];

            switch (tokens[token_idx].type)
            {
            case TOKEN_REDIRECT_OUT:
                redir->type = REDIRECT_OUTPUT;
                redir->fd = STDOUT_FILENO;
                break;
            case TOKEN_REDIRECT_APPEND:
                redir->type = REDIRECT_APPEND;
                redir->fd = STDOUT_FILENO;
                break;
            case TOKEN_REDIRECT_IN:
                redir->type = REDIRECT_INPUT;
                redir->fd = STDIN_FILENO;
                break;
            case TOKEN_REDIRECT_ERR:
                redir->type = REDIRECT_ERROR;
                redir->fd = STDERR_FILENO;
                break;
            case TOKEN_REDIRECT_BOTH:
                redir->type = REDIRECT_BOTH;
                redir->fd = STDOUT_FILENO;
                break;
            default:
                goto end_redirections;
            }

            token_idx++;

            if (tokens[token_idx].type == TOKEN_WORD)
            {
                redir->filename = strdup(tokens[token_idx].value);
                token_idx++;
                current_cmd->redirect_count++;
            }
            else
            {
                DEBUG_ERROR("Expected filename after redirection");
                goto end_redirections;
            }
        }

    end_redirections:

        current_cmd->next_operator = OP_NONE;

        // Check for background or pipeline continuation
        if (tokens[token_idx].type == TOKEN_BACKGROUND)
        {
            current_cmd->is_background = 1;
            current_cmd->next_operator = OP_BACKGROUND;
            token_idx++;
        }
        else if (tokens[token_idx].type == TOKEN_PIPE)
        {
            current_cmd->next_operator = OP_PIPE;
            token_idx++;
        }
        else if (tokens[token_idx].type == TOKEN_SEMICOLON)
        {
            current_cmd->next_operator = OP_SEMICOLON;
            token_idx++;
        }
        else if (tokens[token_idx].type == TOKEN_AND)
        {
            current_cmd->next_operator = OP_AND;
            token_idx++;
            DEBUG_VERBOSE("Found && operator");
        }
        else if (tokens[token_idx].type == TOKEN_OR)
        {
            current_cmd->next_operator = OP_OR;
            token_idx++;
            DEBUG_VERBOSE("Found || operator");
        }

        // Skip empty commands
        if (arg_count > 0)
        {
            cmd_list->count++;
        }
    }

    free(expanded_input);

    // Free tokens
    for (int i = 0; i < token_count; i++)
    {
        if (tokens[i].value)
        {
            free(tokens[i].value);
        }
    }
}

void free_command_list(command_list_t *cmd_list)
{
    for (int i = 0; i < cmd_list->count; i++)
    {

        // free args
        for (int j = 0; cmd_list->commands[i].args[j] != NULL; j++)
        {
            free(cmd_list->commands[i].args[j]);
        }

        // Free redirection filenames
        for (int j = 0; j < cmd_list->commands[i].redirect_count; j++)
        {
            free(cmd_list->commands[i].redirections[j].filename);
        }
    }
    cmd_list->count = 0;
}
