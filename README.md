# ft_irc

## The IRC Client Specification

The protocol as described herein is for use only with client to server connections. Basically some interesting points that I found while reading **RFC2812**.

### Character codes

Because of IRC's Scandinavian origin, the characters `{}|^` are considered to be the lower case equivalents of the characters `[]\~`, respectively. This is a critical issue when determining the equivalence of two nicknames or channel names.

### Messages

Servers and clients send each other messages, which may or may not generate a reply.  If the message contains a valid command, as described in later sections, the client should expect a reply as specified but it is not advised to wait forever for the reply; client to server and server to server communication is essentially asynchronous by nature.

Each IRC message may consist of up to three main parts:

1. **prefix** (optional)
2. **command**
3. **command parameters** (maximum of fifteen)

The prefix, command, and all parameters are separated by one ASCII space character each.


## Naming Convention

### Client

Client's nickname are non-empty strings with the following restrictions:

- They **MUST NOT** contain any of the following characters:

  | NAME | VALUE |
  | :---: | :--: |
  | `SPACE` | ` ` |
  | `COMMA` | `,` |
  | `ASTERISK` | `*` |
  | `QUESTION MARK` | `?` |
  | `EXCLAMATION MARK` | `!` |
  | `AT SIGN` | `@` |

- They **MUST NOT** start with any of the following characters:

  | NAME | VALUE |
  | :---: | :--: |
  | `DOLLAR` | `$` |
  | `COLON` | `:` |

- They **MUST NOT** start with a character listed as a *channel type* prefix:

  | NAME | VALUE |
  | :---: | :--: |
  | `HASH SIGN` (Regular channel) | `#` |
  | `AND SIGN` (Local channel) | `&` |

- They **SHOULD NOT** contain any dot character (`.`).

### Channel

Channel names should start with the *channel type* prefix (`#` or `&`). Besides that, channel name may not contain:

| NAME | VALUE |
| :--: | :--: |
| `SPACE` | ` ` |
| `CONTROL G/BELL` | `^G` |
| `COMMA` | `,` |
