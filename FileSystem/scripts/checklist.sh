# /bin/sh
# 检查库和模块的列表文件存在与否
# 李志杰 2006.07.11

	AS600_MODULE_LIST=$ROOT_DIR/scripts/as600Modules.list
	if [ ! -f "$AS600_MODULE_LIST" -o ! -r "$AS600_MODULE_LIST" ] ; then
		$ECHO " No AS600 Modules List is found" >& 2
		exit 1
	else
		. $AS600_MODULE_LIST
	fi


	COMMON_LIB_LIST=$ROOT_DIR/scripts/commonLibs.list
	if [ ! -f "$COMMON_LIB_LIST" -o ! -r "$COMMON_LIB_LIST" ] ; then
		$ECHO " No Common Libraries List is found" >& 2
		exit 1
	else
		. $COMMON_LIB_LIST
	fi

	COMMON_PROGS_LIST=$ROOT_DIR/scripts/commonProgs.list
	if [ ! -f "$COMMON_PROGS_LIST" -o ! -r "$COMMON_PROGS_LIST" ] ; then
		$ECHO " No Common Programs List is found" >& 2
		exit 1
	else
		. $COMMON_PROGS_LIST
	fi

