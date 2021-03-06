RCSID $Id: include.linux.in.h,v 1.1.1.1 2006/11/29 08:55:38 lizhijie Exp $
--- ./include/linux/in.h.preipsec	Tue Dec  2 17:18:11 1997
+++ ./include/linux/in.h	Mon Apr  5 05:33:07 1999
@@ -31,6 +31,9 @@
   IPPROTO_PUP = 12,		/* PUP protocol				*/
   IPPROTO_UDP = 17,		/* User Datagram Protocol		*/
   IPPROTO_IDP = 22,		/* XNS IDP protocol			*/
+  IPPROTO_ESP = 50,		/* Encapsulation Security Payload protocol */
+  IPPROTO_AH = 51,		/* Authentication Header protocol	*/
+  IPPROTO_COMP = 108,		/* Compression Header protocol		*/
 
   IPPROTO_RAW = 255,		/* Raw IP packets			*/
   IPPROTO_MAX
