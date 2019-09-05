# 图标
RC_ICONS = version/ApplicationVersion.ico
# 语言
# 0x0004 表示 简体中文
# 详见 https://msdn.microsoft.com/en-us/library/dd318693%28vs.85%29.aspx
RC_LANG = 0x0004

# 公司名
QMAKE_TARGET_COMPANY = XXXX公司
# 产品名称
QMAKE_TARGET_PRODUCT = 测试应用
# 详细描述
QMAKE_TARGET_DESCRIPTION = XXXXXX描述
# 版权
QMAKE_TARGET_COPYRIGHT = Copyright(C) 2018 XXXXX

# 辅助rc版本
HEADERS += \
    $$PWD/version.h
