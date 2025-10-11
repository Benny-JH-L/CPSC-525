/// -------------------------------------------------------------------------------------
/// you need to edit this file, and this is the only file you need to edit
/// -------------------------------------------------------------------------------------
///
/// (c) 2025, Pavol Federl, pfederl@ucalgary.ca
/// Do not distribute this file.
///
/// If you collaborated on this assignment with another student,
/// list their name here: ________________________________
///
/// -------------------------------------------------------------------------------------

/// Benny Liang | UCID: 30192142 | Assignment 2 - Coding Part

#include "checkp.h"
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <vector>

using namespace std;
int validatePermsString(string inputPerms, mode_t& mode);
void splitPathToComponents(string path, vector<string> &componentPaths);
bool isGroupInUserGroups(gid_t groupToCheck, gid_t *userGroups, int* nGroups);

// anonymous namespace for internal helper functions:
namespace {

/// @brief Returns a debug output stream if debugging is enabled. Otherwise returns a null stream.
/// @details Debugging is enabled if the environment variable CHECKP_DEBUG is set to 1,
///   true or TRUE.
/// You do not have to use this function. It is here to help you debug your code.
/// If you use some other way to print debug information, that is fine too, just make sure
/// when you submit your code for grading, it does not print any debug information!
std::ostream & dbg()
{
    static std::string debug_env = getenv("CHECKP_DEBUG") ? getenv("CHECKP_DEBUG") : "";
    static bool debug = debug_env == "1" || debug_env == "true" || debug_env == "TRUE";
    static std::ostream null_stream(nullptr);
    return debug ? std::cout : null_stream;
}

} // namespace

/// @brief Checks whether file or directory specified by `path` is accessible to user `username`
///   with the given permissions `perms`.
/// @param username valid user name, must exist, and root is handled like any other user
/// @param perms requested permissions (must only contain letters 'r', 'w' or
///   'x', and must not be empty)
/// @param path file or directory path (must be absolute)
/// @return
///    1 if the user definitely has the requested permissions,
///    0 if the user definitely does not have requested permissions,
///   -1 if the permissions cannot be checked or an error occured
///
/// In case of error, the function sets errno appropriately:
///   EEXIST  : username does not exist, i.e. getpwnam(3) failed
///   ESRCH   : user's groups could not be retrieved, i.e. getgrouplist(3) failed
///   EINVAL  : invalid permissions string, i.e. contains bad characters or is empty
///   ENOTSUP : if path is not absolute, i.e. does not start with '/'
///   ENOENT  : some component of path or the file does not exist, i.e. lstat(2) failed with ENOENT
///   *       : if lstat(2) fails, simply propagate errno as set by lstat(2)
int checkp(const std::string & username, const std::string & perms, const std::string & path)
{
    // no error yet
    errno = 10;

    // make sure path is absolute
    if (path.empty() || path[0] != '/') {
        errno = ENOTSUP; // relative paths not supported
        return -1;
    }

    // lstat() -> gets file metadata

    // implement the rest of the function, following this algorithm:
    //   - the only system call you should use is lstat(2)
    //   - the only libc functions related to retrieving user IDs and group IDS you may use are
    //     getpwnam(3) and getgrouplist(3)
    //   - you can define and use any helper functions you need
    //   - you can define and use any data structures you need
    //   - you can use any C++ standard library containers
    // ============================================================================================
    // find userid and primary groupid for username
    //   - use getpwnam(3) for this

    passwd* pw = getpwnam(username.c_str());
    if (pw == nullptr)
    {
        // cout << "getpwnam("<<username << ") FAILED" << endl;    // debug
        errno = EEXIST;
        return -1;
    }
    uid_t userID = pw->pw_uid;
    gid_t userGID = pw->pw_gid;
    // cout << "userID: " << userID <<
    // "\nuser group id: " << userGID << endl; // debug

    // retrieve all groups for userid using getgrouplist(3)
    //   - you need to call it twice as described in its man page
    //   - first time with ngroups=0 to get number of groups
    //   - second time with allocated array of gid_t to get actual groups
    //     - note: the number of groups may change between the two calls,
    //       in which case you should return -1 with errno set to ESRCH

    gid_t *userGroups = (gid_t *) malloc(sizeof(0));
    int nGroups = 0;
    // int result = getgrouplist(username.c_str(), userGID, groups, &nGroups);   

    if (getgrouplist(username.c_str(), userGID, userGroups, &nGroups) == -1)
    {
        userGroups = (gid_t *) malloc(sizeof(nGroups));
        int nGroupsCopy = nGroups;
        if (getgrouplist(username.c_str(), userGID, userGroups, &nGroups) != nGroupsCopy)
        {
            // cout << "getgrouplist() 2nd call and result != nGroups (number of groups changed between this and the 1st call)" << endl; // debug
            free(userGroups);
            errno = ESRCH;
            return -1;
        }
    }

    // debugging
    // cout << "nGroup size = " << nGroups << endl; 
    // for (int i = 0; i < nGroups; i++)
    //     cout << userGroups[i] << "\n";
    // cout << endl;

    // ============================================================================================
    // validate perms string (must not be empty, and must only contain 'r', 'w' and/or 'x')
    //   - if invalid, return -1 with errno set to EINVAL
    //   - if valid, convert perms into mode_t (bit mask of requested permissions)

    mode_t userPerms = 0;    // note, `mode_t` will show perms as decimal (base 10).
    if (validatePermsString(perms, userPerms) == -1)
    {
        // cout << "INVALID PERMS" << endl; // debug
        errno = EINVAL;
        return -1;
    }

    // cout << "Valid perms <"<< perms <<">: mode<0"<< oct << userPerms << dec << ">" << endl; // debug (print the `mode` in octal then go back to decimal)
     
    // split path into components, starting with root, and not including the full path:
    //   - e.g. /a/b/c/d.txt -> /, /a, /a/b, /a/b/c
    vector<string> componentPaths;
    splitPathToComponents(path, componentPaths);

    // debugging
    // cout << "Component paths: \n";
    // for (string s : componentPaths)
    // {
    //     cout << s << "\n";
    // }
    // cout << endl;

    // for each path component, check if user has execute permission
    //   - use lstat(2) to get file status (ownership and permissions)
    //   - if file is owned by username, check owner's permissions for execute
    //     - if owner does not have execute access, immediately return 0
    //   - else if file is owned by one of user's groups, check group permissions for execute
    //     - if group does not have execute access, immediately return 0
    //   - else check other permissions for execute
    //     - if others do not have execute access, immediately return 0
    
    // go thorough all path components (in `componentPaths`)
    struct stat fileStatus;
    uid_t fileOwnerID;
    gid_t fileGroupID;
    mode_t fileMode;

    for (string compPath : componentPaths)
    {
        if (lstat(compPath.c_str(), &fileStatus) == -1)
        {
            // cout << "error occured in lstat("<<compPath<<") going through COMPONENT paths" << endl;   // debug
            return -1;
        }

        fileOwnerID = fileStatus.st_uid;
        fileGroupID = fileStatus.st_gid;
        fileMode = fileStatus.st_mode;

        // debugging
        // cout << "lstat(" << compPath << ")" <<
        // " file owner's id: " << fileOwnerID <<                 
        // " Group ID of the file's group: " << fileGroupID <<     
        // " File mode: " << oct << fileMode << dec                          // print the mode's in octal, then go back to decimal
        // << endl;    

        // check if user is in the group(s) of the file
        // bool userInFileGroup = false;

        // for (int i = 0; i < nGroups; i++)
        // {
        //     if (userGroups[i] == fileGroupID)
        //     {
        //         userInFileGroup = true;
        //         break;
        //     }
        // }

        // check permissions
        if (userID == fileOwnerID)
        {
            if ((fileMode & S_IXUSR) == 0)  // check execute bit for `owner`
                return 0;
        }
        else if (isGroupInUserGroups(fileGroupID, userGroups, &nGroups)) // (userInFileGroup) // (userGID == fileGroupID)    
        {
            if ((fileMode & S_IXGRP) == 0)  // check execute bit for `group`
                return 0;
        }
        else
        {
            if ((fileMode & S_IXOTH) == 0)  // check execute bit for `other`
                return 0;
        }

        // cout << "\tuser valid permissions" << endl; // debug
    }

    // check if user has requested permissions as specified by `perms` on the full path
    //   - use lstat(2) to get file status
    //   - if file is owned by username, check user owner permissions
    //     - if owner permissions support all requested permissions, return 1
    //     - else return 0
    //   - else if file is owned by one of user's groups, check group permissions
    //     - if group permissions support all requested permissions, return 1
    //     - else return 0
    //   - else check other permissions
    //     - if others permissions support all requested permissions, return 1
    //     - else return 0
    // Note: in all cases of error, return -1 with errno set appropriately

    if (lstat(path.c_str(), &fileStatus) == -1)
    {
        // cout << "error occured in lstat("<<path<<") going through FULL path" << endl;   // debug
        return -1;
    }

    mode_t requiredPerms = 0;
    fileOwnerID = fileStatus.st_uid;
    fileGroupID = fileStatus.st_gid;
    fileMode = fileStatus.st_mode;

    if (userID == fileOwnerID)
    {
        if ((fileMode & userPerms) == userPerms)
            return 1;
        return 0;
    }
    else if (isGroupInUserGroups(fileGroupID, userGroups, &nGroups))
    {
        // convert user permissions to `group` permissions
        if (userPerms & S_IRUSR) 
            requiredPerms |= S_IRGRP;
        if (userPerms & S_IWUSR) 
            requiredPerms |= S_IWGRP;
        if (userPerms & S_IXUSR) 
            requiredPerms |= S_IXGRP;
        
        if ((fileMode & requiredPerms) == requiredPerms)    // user has requested perms 
            return 1;
        return 0;
    }
    else
    {
        // convert user permissions to `other` permissions
        if (userPerms & S_IRUSR) 
            requiredPerms |= S_IROTH;
        if (userPerms & S_IWUSR) 
            requiredPerms |= S_IWOTH;
        if (userPerms & S_IXUSR) 
            requiredPerms |= S_IXOTH;
        
        if ((fileMode & requiredPerms) == requiredPerms)    // user has requested perms 
            return 1;
        return 0;
    }

    free(userGroups);   // free memory

    // dbg() << "checkp('" << username << "', '" << perms << "', '" << path << "')\n";
    // dbg() << "NOT IMPLEMENTED YET\n";

    errno = 65535; // invalid errno?
    return -1;
}


    // find userid and primary groupid for username
    //   - use getpwnam(3) for this
    // retrieve all groups for userid using getgrouplist(3)
    //   - you need to call it twice as described in its man page
    //   - first time with ngroups=0 to get number of groups
    //   - second time with allocated array of gid_t to get actual groups
    //     - note: the number of groups may change between the two calls,
    //       in which case you should return -1 with errno set to ESRCH
    // ============================================================================================
    // validate perms string (must not be empty, and must only contain 'r', 'w' and/or 'x')
    //   - if invalid, return -1 with errno set to EINVAL
    //   - if valid, convert perms into mode_t (bit mask of requested permissions)
    // split path into components, starting with root, and not including the full path:
    //   - e.g. /a/b/c/d.txt -> /, /a, /a/b, /a/b/c
    // for each path component, check if user has execute permission
    //   - use lstat(2) to get file status (ownership and permissions)
    //   - if file is owned by username, check owner's permissions for execute
    //     - if owner does not have execute access, immediately return 0
    //   - else if file is owned by one of user's groups, check group permissions for execute
    //     - if group does not have execute access, immediately return 0
    //   - else check other permissions for execute
    //     - if others do not have execute access, immediately return 0
    // check if user has requested permissions as specified by `perms` on the full path
    //   - use lstat(2) to get file status
    //   - if file is owned by username, check user owner permissions
    //     - if owner permissions support all requested permissions, return 1
    //     - else return 0
    //   - else if file is owned by one of user's groups, check group permissions
    //     - if group permissions support all requested permissions, return 1
    //     - else return 0
    //   - else check other permissions
    //     - if others permissions support all requested permissions, return 1
    //     - else return 0
    // Note: in all cases of error, return -1 with errno set appropriately

/// @brief Validates the permission string entered.
/// @param inputPerms a string, the perms entered.
/// @param mode a reference to a `mode_t` type
/// @return an int. Returns -1 if the permissions are invalid, and sets `mode` to 0. If valid returns 1.
int validatePermsString(string inputPerms, mode_t& mode)
{
    mode_t myMode = 0;
    int returnVal = 1;
    // cout << "entred: validatePermsString()" << endl; // debug

    // check for empty string
    if (inputPerms.empty())
        returnVal = -1;
    
    // iterate through `inputPerms` and verify it only contains 'r', 'w' and/or 'x'.
    for (auto itBegin = inputPerms.begin(); itBegin != inputPerms.end(); itBegin++)
    {
        // cout << *itBegin << endl;
        if (*itBegin == 'r')
        {
            myMode |= S_IRUSR;  // add read perm
        }
        else if (*itBegin == 'w')
        {
            myMode |= S_IWUSR;  // add write perm
        }
        else if (*itBegin == 'x')
        {
            myMode |= S_IXUSR;  // add execute perm
        }
        // non 'r', 'w' and/or 'x' character
        else
        {
            returnVal = -1;
            myMode = 0;
            break;
        }
    }

    mode = myMode;
    // cout << "exiting: validatePermsString()" << endl; // debug
    return returnVal;
}

/// @brief Splits the string `path` into components stored into a vector<string>, ex. path = /a/b/c/d.txt, turns it into;  /, /a, /a/b, /a/b/c.
/// @param path 
/// @param componentPaths 
void splitPathToComponents(string path, vector<string> &componentPaths)
{
    // cout << "entering: splitPathToComponents()" << endl; // debug

    componentPaths.clear();     // ensure it is empty
    // iterate through path
    string component = "";
    for (auto itBegin = path.begin(); itBegin != path.end(); itBegin++)
    {
        component += (*itBegin);    // add the character (*itBegin) to `component`
        if ((*itBegin) == '/')      // add this component to the `vector<string>`
        {
            componentPaths.emplace_back(component);
        }
    }

    // remove trailing slashes, ex. /home/ -> remove the right most `/`
    for (string& component : componentPaths)
    {
        if (component.size() > 1)   // not `root` component path
            component.erase(component.size() - 1, 1);   // removing the trailing `/` (ie last character of the string)
    }
    // cout << "exiting: splitPathToComponents()" << endl; // debug
}

/// @brief Checks if `groupToCheck` is inside `userGroups`.
/// @param groupToCheck the gid_t to check.
/// @param userGroups a pointer to an array of gid_t.
/// @param nGroups the number of gid_t in the array `userGroups`.
/// @return true if `groupToCheck` is in `userGroups`, false otherwise.
bool isGroupInUserGroups(gid_t groupToCheck, gid_t *userGroups, int* nGroups)
{
    for (int i = 0; i < *nGroups; i++)
    {
        if (userGroups[i] == groupToCheck)
        {
            return true;
        }
    }
    return false;
}
