/**********************************************************************
* 版权所有 (C)2015, Zhou Zhaoxiong。
*
* 文件名称：WriteFileHeader.c
* 文件标识：无
* 内容摘要：测试文件头的写入
* 其它说明：无
* 当前版本：V1.0
* 作    者：Zhou Zhaoxiong
* 完成日期：20150113
*
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// 重定义数据类型
typedef signed   char       INT8;
typedef unsigned char       UINT8;
typedef unsigned int        UINT32;
typedef signed   int        INT32;

// 函数声明
void WriteToFile(UINT8 *pszContentLine);

/**********************************************************************
* 功能描述：主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号     修改人            修改内容
* -------------------------------------------------------------------
* 20150113        V1.0     Zhou Zhaoxiong        创建
***********************************************************************/
INT32 main()
{
    UINT8  szContentLine[1000] = {0};

    // 第一次写文件
    // 拼装写本地文件的内容
    snprintf(szContentLine, sizeof(szContentLine)-1, "1234|abcd|\r\n");
    // 将文件内容写入本地文件
    WriteToFile(szContentLine);
	
    // 第二次写文件
    // 拼装写本地文件的内容
    snprintf(szContentLine, sizeof(szContentLine)-1, "5678|efgh|\r\n");
    // 将文件内容写入本地文件
    WriteToFile(szContentLine);

    // 第三次写文件
    // 拼装写本地文件的内容
    snprintf(szContentLine, sizeof(szContentLine)-1, "4321|dcba|\r\n");
    // 将文件内容写入本地文件
    WriteToFile(szContentLine);

    return 0;                  // main函数返回0
}


/**********************************************************************
 * 功能描述： 写本地文件, 同时更新文件头
 * 输入参数： pszContentLine: 一条文件记录
 * 输出参数： 无
 * 返 回 值： 无
 * 其它说明： 文件头的长度为20字节, 前10字节为写入文件内容的大小, 后10字节为文件内容的条数
 * 修改日期            版本号            修改人           修改内容
 * ----------------------------------------------------------------------
*  20150113             V1.0          Zhou Zhaoxiong        创建
 ************************************************************************/
void WriteToFile(UINT8 *pszContentLine)
{
    INT8   szFileName[500]     = {0};
    FILE  *fp                  = NULL;
    UINT8  iIsNewCdrFile       = 0;
    UINT8  szFileHeader[100]   = {0};
    UINT8  szTmpBuf[50]        = {0};
    UINT32 iSize               = 0;
    UINT32 iCount              = 0;
    UINT32 iLen                = 0;
    
    if (NULL == pszContentLine)
    {
        printf("WriteToFile: input parameter is NULL.\n");
        return;
    }
    
    snprintf(szFileName, sizeof(szFileName)-1, "/home/zhou/zhouzhaoxiong/TestFile.txt");
    
    // 判断是否为新文件
    if (access(szFileName, 0) == -1)
    {
        iIsNewCdrFile = 1;     // 是新文件
    }
    else
    {
        iIsNewCdrFile = 0;
    }

    fp = fopen(szFileName, "a+");
    if (fp == NULL)
    {
         printf("WriteToFile: open file failed, file=%s\n", szFileName);
         return;
    }

    // 如果是新文件, 先写文件头
    if (iIsNewCdrFile == 1)
    {
        // 新话单文件, 写入文件头
        memset(szFileHeader, 0x00, sizeof(szFileHeader));
        memset(szFileHeader, ' ', 20);    
        strcat(szFileHeader, "\r\n");       // 回车换行符
	      // 文件头第一部分, 文件内容的大小
        sprintf(szTmpBuf, "%d", strlen(pszContentLine)-2);   // 要去掉最后的回车换行符的大小, 因此这里要减去2
        memcpy(szFileHeader, szTmpBuf, strlen(szTmpBuf));

      	// 文件头第二部分, 文件内容的条数
        sprintf(szTmpBuf, "%d", 1);   // 写入第一条
        memcpy(szFileHeader+10, szTmpBuf, strlen(szTmpBuf));

        printf("WriteToFile: now write header to file, Count of content=%s\n", szTmpBuf);
        fputs(szFileHeader, fp);
    }

    fputs(pszContentLine, fp);
    fflush(fp);

    fclose(fp);
    fp = NULL;     // 写入完毕要在关闭文件的同时置文件指针为空

    // 更新文件头
    if (iIsNewCdrFile == 0)       // 文件已存在
    {
        fp = fopen(szFileName, "r+");
        if (fp == NULL)
        {
             printf("WriteToFile: open file for updating header failed, file=%s\n", szFileName);
             return;
        }
    
        // 已有话单文件, 更新文件头
        memset(szFileHeader, 0x00, sizeof(szFileHeader));
        fseek(fp, 0, SEEK_SET);        // 文件第一行
        fread(szFileHeader, 20, 1, fp);
        fflush(fp);
        
	      // 更新文件内容的大小
        memset(szTmpBuf, 0x00, sizeof(szTmpBuf));
        memcpy(szTmpBuf, szFileHeader, 10);
        iSize = atoi(szTmpBuf);    // 原大小
        sprintf(szTmpBuf, "%d", iSize+strlen(pszContentLine)-2);
        iLen = strlen(szTmpBuf);
        memcpy(szFileHeader, szTmpBuf, iLen);

	      // 更新文件内容的条数
        memset(szTmpBuf, 0x00, sizeof(szTmpBuf));
        memcpy(szTmpBuf, szFileHeader+10, 10);
        iCount = atoi(szTmpBuf);
        sprintf(szTmpBuf, "%d", iCount+1);
        iLen = strlen(szTmpBuf);
        memcpy(szFileHeader+10, szTmpBuf, iLen);

        printf("WriteToFile: now update file header, Count of content=%s\n", szTmpBuf);
        fseek(fp, 0, SEEK_SET);
        fwrite(szFileHeader, 20, 1, fp);
        fflush(fp);

        fclose(fp);
        fp = NULL;
    }
}
