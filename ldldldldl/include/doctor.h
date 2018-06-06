#ifndef _DOCTOR_H
#define _DOCTOR_H

/***********  医生登录选择  **********/
int login_menu(int sockfd);

/***************  医生登录验证  *********/
int login(int sockfd);


/**************** 医生操作界面  *********/
int doc_menu(int sockfd);

/***************  医生诊疗病人  *********/
void doc_cur_pat(int sockfd);

/**************  医生找要诊疗的病人 ********/
int find_needmed(int sockfd,med *old_med);


/**************  医生找需要使用的项目  **********/
int find_needassay(int sockfd,assay *old_assay);


/**************  医生找需要使用的药品  **********/
int find_curepat(int sockfd,pat *old_pat);


/****************  医生查看所有的患者信息  ***********/
void doc_show_allpat(int sockfd);

/****************  医生查看等待患者信息  ***********/
void doc_show_unpat(int sockfd);

#endif
