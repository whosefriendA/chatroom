#include"./server.hpp"
void transferfunc(TaskSocket asocket,const string& comad_string)
{   Message msg;
    msg.Json_to_s(comad_string);
    switch((int)msg.flag){
        case SIGNUP:
            Sign_up(asocket,msg);
            break;
        case LOGIN:
            Log_in(asocket,msg);
            break;
        case USER_REMOVE:
            User_remove(asocket,msg);
            break;
        case QUESTION_GET:
            question_get(asocket,msg);
            break;
        case PASSWORD_FIND:
            pass_find(asocket,msg);
            break;
        case PASSWORD_GET:
            pass_get(asocket,msg);
            break;
        case FRIEND_LIST:
            friend_list_get(asocket,msg);
            break;
        case ADD_FRIEND:
            friend_add(asocket,msg);
            break;
        case DELETE_FRIEND:
            friend_del(asocket,msg);
            break;
        case AGREE_ADDFRIEND:
            friend_apply_agree(asocket,msg);
            break;
        case REFUSE_ADDFRIEND:
            friend_apply_refuse(asocket,msg);
            break;
        case  SHIELD_FRIEND:
            friend_shield(asocket,msg);
            break;
        case RESTORE_FRIEND:
            friend_restore(asocket,msg);
            break;
        case CHAT_WITH_FRIEND:
            friend_chat(asocket,msg);
            break;
        case EXITCHAT:
            Exitchat(asocket,msg);
            break;
        case SEND_MSG:
            friend_sendmsg(asocket,msg);
            break;
        case F_SENDFILE:
            F_sendfile_toserver(asocket,msg);
            break;
        case F_RECVFILE:
            F_receivefile_fromserver(asocket,msg);
            break;
        case UNREAD_NOTICE:
            Unreadnotice(asocket,msg);
            break;
        case CREATE_GROUP:
            group_create(asocket,msg);
            break;
        case ADD_GROUP:
            group_add(asocket,msg);
            break;
        case GROUP_IN:
            group_in(asocket,msg);
            break;
        case GROUP_LIST:
            group_list_get(asocket,msg);
            break;
        case GROUP_MEMBER_LIST:
            group_memberlist_get(asocket,msg);
            break;
        case GROUP_APPLY_LIST:
            group_apply_list(asocket,msg);
            break;
        case GROUP_APPLY_AGREE:
            group_apply_agree(asocket,msg);
            break;
        case GROUP_APPLY_REFUSE:
            group_apply_refuse(asocket,msg);
            break;
        case GROUP_MANAGER_SET:
            group_manager_set(asocket,msg);
            break;
        case GROUP_MANAGER_UNSET:
            group_manager_unset(asocket,msg);
            break;
        case GROUP_DISBAND:
            group_disband(asocket,msg);
            break; 
        case GROUP_EXIT:
            group_exit(asocket,msg);
            break;
        case GROUP_DEL_MEMBER:
            group_delmember(asocket,msg);
            break;
        case GROUP_CHAT:
            group_chat(asocket,msg);
            break;
        case GRUOP_CHATEXIT:
            group_chatexit(asocket,msg);
            break;
        case GROUP_SENDMSG:
            group_sendmsg(asocket,msg);
            break;
        case SENDFILE_GROUP:
            G_sendfile_toserver(asocket,msg);
            break;
        case RECVFILE_GROUP:
            G_receivefile_fromserver(asocket,msg);
            break;
    }
    return;
}