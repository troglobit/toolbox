#!/bin/sh
#
# ignore any ~/.indent.pro
#
# With the -T <type> we can inform indent about non-ansi types
# that we've added, so indent doesn't insert spaces in odd places. 
# 
#       --line-length132							
#       --comment-line-length132                                                 
indent -linux									\
       --ignore-profile								\
       --preserve-mtime								\
       --break-after-boolean-operator						\
       --blank-lines-after-procedures						\
       --blank-lines-after-declarations						\
       --dont-break-function-decl-args						\
       --dont-break-procedure-type						\
       --leave-preprocessor-space						\
       --honour-newlines							\
       --space-after-if								\
       --space-after-for							\
       --space-after-while							\
       --leave-optional-blank-lines						\
       --format-all-comments                                                    \
       --dont-left-justify-declarations                                         \
       --line-length90							        \
       --comment-line-length90                                                 \
       --comment-indentation49                                                  \
       --else-endif-column1                                                     \
       --no-blank-lines-after-commas						\
       --no-space-after-parentheses						\
       --no-space-after-casts							\
       -T size_t -T sigset_t -T timeval_t -T pid_t -T pthread_t -T FILE		\
       -T time_t -T uint32_t -T uint16_t -T uint8_t -T uchar -T uint -T ulong	\
       -T u_int32_t -T u_int16_t -T u_int8_t 	\
       -T xht_t -T xhn_t -T throttletab                                         \
       -T Timer -T ClientData -T TimerProc                                      \
       -T httpd_sockaddr -T httpd_server -T httpd_conn -T Map -T connecttab	\
       -T libnet_t -T FileData -T ETHERhdr -T IPhdr -T UDPhdr -T ICMPhdr -T IGMPhdr \
$*
