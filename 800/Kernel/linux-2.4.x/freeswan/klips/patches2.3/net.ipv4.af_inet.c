RCSID $Id: net.ipv4.af_inet.c,v 1.1.1.1 2006/11/30 16:28:46 lizhijie Exp $
--- ./net/ipv4/af_inet.c.preipsec	Wed Apr 26 15:13:17 2000
+++ ./net/ipv4/af_inet.c	Fri Jun 30 15:01:27 2000
@@ -1019,6 +1019,17 @@
 	ip_mr_init();
 #endif
 
+#if defined(CONFIG_IPSEC)
+	{
+               extern /* void */ int ipsec_init(void);
+		/*
+		 *  Initialise AF_INET ESP and AH protocol support including 
+		 *  e-routing and SA tables
+		 */
+		ipsec_init();
+	}
+#endif /* CONFIG_IPSEC */
+
 	/*
 	 *	Create all the /proc entries.
 	 */
