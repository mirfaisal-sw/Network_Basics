#!/bin/sh
#=============================================================================
# HEADER
#=============================================================================
#% Author : Archandran <Arjun.Chandran@harman.com>
#%
#% SYNOPSIS
#%    ${SCRIPT_NAME} [-f <from_date>] [-t <to_date] args ...
#%
#% DESCRIPTION
#%    This script can be used to find lines of code modifications
#%    in a git repo between two dates.(default is last 7days from
#%    current date)
#% OPTIONS
#%    -r <git_repo_directory> (optional : default current dir)
#%    -f <from_date> (optional : yyyy-mm-dd format)
#%    -t <to_date> (optional : yyyy-mm-dd format : default current date)
#%    -d <duration> (optional : default 7 days)
#%    -a author1,author2,author3 ( To list changes added by specific
#%       authors)
#%    -c commithash_1 commithash_2 (provides loc changes between commits)
#%    -h (show help)
#% EXAMPLES
#%    ${SCRIPT_NAME} -r ../git_repo -d 15
#%    sh analyze_git_repo.sh -r repo_name  -a author1,author2,author3 -d 200
#%    above command lists the changes added by author1,author2,author3 in
#%    last 200 days in git_repo
#%
#============================================================================

from_date=""
to_date=""
duration=""
git_repo=""


show_help() {
    echo    " Usage : $0 -f <from_date> (optional : yyyy-mm-dd format)
	-t <to_date> (optional : yyyy-mm-dd format : default current date)
	-r <git_repo_directory> (optional : default current dir)
	-d <duration> (optional : default 7 days)
        -a author1,author2,author3 ( To list changes added by specific authors)
        -c commithash_1 commithash_2 (provides loc changes between commits) "
}

function analyse_commit_by_author()
{
    echo -e "\n##########################################################"
    echo " Report of commits done by the Author  $authorlist"
    if [ -z $git_branch ]
    then
        echo -e "\n\t Git Repo path = $git_repo \n\t tag = $git_tag"
    else
        echo -e "\n\t Git Repo path = $git_repo \n\t Remote branch = $git_branch"
    fi
    echo -e "\n\t Over the period from $from_date to $to_date "
    echo -e "##########################################################\n"

    for author in $authors
    do
        echo -e "\t###Author### \t\t ###Commit### \t\t\t\t\t ###Lines Modified### \t\t ###Short Description###"
        git_commits=`git --git-dir="$git_repo""/.git"  log --pretty=format:"Commit Hash: %H"  --before={$to_date} --after={$from_date} --author=$author |  cut -d':' -f2`

        if [ "$git_commits" == "" ]
        then
            echo -e "\n\t  No commits from Author $author !!!\n"
            continue
        fi

        total_insert=0
        total_delete=0
        commit_insert=0
        commit_delete=0
        commit_count=0

        for commits in $git_commits
	do
            commit_count=$(expr $commit_count + 1)
            commit_insert=`git --git-dir="$git_repo""/.git" show --stat  $commits | tail -1 | cut -d',' -f 2 |sed 's/^\s*//'|cut -d' ' -f1`
            if [ $? -eq 0 ]
            then
                if [ ! -z $commit_insert ]
                then
                    total_insert=$(expr $total_insert + $commit_insert)
                else
                    commit_insert=0
                fi
            fi

            commit_delete=`git --git-dir="$git_repo""/.git" show --stat  $commits | tail -1 | cut -d',' -f 3|sed 's/^\s*//'|cut -d' ' -f1`
            if [ $? -eq 0 ]
            then
                if [ ! -z $commit_delete ]
                then
                    total_delete=$(expr $total_delete + $commit_delete)
                else
                    commit_delete=0
                fi
            fi
            commit_description=`git --git-dir="$git_repo""/.git" log --pretty=oneline --format=%B -n 1 $commits | head -n 1`
            printf "\t%-10s" "$author"
            echo -e " \t\t $commits \t insertions=$commit_insert, deletions=$commit_delete \t ${commit_description:0:60}"
        done

        echo -e "\n Total commits by author $author = $commit_count \n Lines Inserted = $total_insert , Lines Deleted = $total_delete \n\n"

    done
}

function analyse_all_commits()
{
    echo -e "\n##########################################################"
    echo -e "Report for lines of code modifcations for following git repo"
    if [ -z $git_branch ]
    then
        echo -e "\n\t Git Repo path = $git_repo \n\t tag = $git_tag"
    else
        echo -e "\n\t Git Repo path = $git_repo \n\t Remote branch = $git_branch"
    fi
    echo -e "\n\t Over the period from $from_date to $to_date"
    echo -e "##########################################################\n\n"

    git_commits=`git --git-dir="$git_repo""/.git"  log --pretty=format:"Commit Hash: %H"  --before={$to_date} --after={$from_date} |  cut -d':' -f2`

    if [ "$git_commits" == "" ]
    then
        echo -e "No commits, changes in the specified period !!!\n\n"
        exit 1
    fi


    total_insert=0
    total_delete=0
    commit_insert=0
    commit_delete=0

    for commits in $git_commits
    do
        commit_insert=`git --git-dir="$git_repo""/.git" show --stat  $commits | tail -1 | cut -d',' -f 2 |sed 's/^\s*//'|cut -d' ' -f1`
        if [ $? -eq 0 ]
        then
            if [ ! -z $commit_insert ]
            then
                total_insert=$(expr $total_insert + $commit_insert)
            fi
        fi
        commit_delete=`git --git-dir="$git_repo""/.git" show --stat  $commits | tail -1 | cut -d',' -f 3|sed 's/^\s*//'|cut -d' ' -f1`
        if [ $? -eq 0 ]
        then
            if [ ! -z $commit_delete ]
            then
                total_delete=$(expr $total_delete + $commit_delete)
            fi
        fi
    done


    git_last_commit=`git --git-dir="$git_repo""/.git"  log --pretty=format:"Commit Hash: %H"  --before={$to_date} --after={$from_date} | tail -1 | cut -d':' -f2`

#echo "first commit = $git_first_commit, git_last_commit = $git_last_commit commit_before_last=$git_commit_before_last"

    git_first_commit=`git --git-dir="$git_repo""/.git"  log --pretty=format:"Commit Hash: %H"  --before={$to_date} --after={$from_date} | head -1 | cut -d':' -f2`

    git_commit_before_last=`git --git-dir="$git_repo""/.git" log $git_last_commit  --pretty=format:"Commit Hash: %H" | head -2|tail -1| cut -d':' -f2`

    combined_insert=`git --git-dir="$git_repo""/.git" diff --stat $git_first_commit  $git_commit_before_last | tail -1 | cut -d',' -f 2 |sed 's/^\s*//'|cut -d' ' -f1`
    combined_delete=`git --git-dir="$git_repo""/.git" diff --stat $git_first_commit  $git_commit_before_last | tail -1 | cut -d',' -f 3 |sed 's/^\s*//'|cut -d' ' -f1`


    echo "        1) Total  modifications adding the changes in each commit
                         Lines Inserted = $total_insert
                         Lines Deleted  = $total_delete"
    echo "        2) overall modifications
                         Lines Inserted = $combined_insert
                         Lines Deleted  = $combined_delete"

    echo -e "\n"
}

while getopts "f:t:d:r:a:c:h" opt; do
    case "$opt" in
    h)
        show_help
        exit 0
        ;;
    f)  from_date=$OPTARG
        ;;
    t)  to_date=$OPTARG
        ;;
    d)  duration=$OPTARG
        ;;
    r)  git_repo=$OPTARG
        ;;
    a)  authorlist=$OPTARG
        ;;
    c)  commit1=$OPTARG
        shift $(( OPTIND - 1 ))
        commit2=`echo $@ | cut -d' ' -f1`
        diff_by_commit=1
        ;;
    *)  echo "Unknown option $opt "
        show_help
        exit 1
        ;;

    esac
done

if [ ! -z $diff_by_commit ]
then
    echo -e "\n##########################################################"
    echo -e "Report for lines of code modifcations for following git repo"
    echo -e "\n\t Git Repo path = $git_repo \n"
    echo -e "\n\t Between commit ids $commit1 and $commit2  \n"
    echo -e "##########################################################\n\n"

    combined_insert=`git --git-dir="$git_repo""/.git" diff --stat $commit1  $commit2 | tail -1 | cut -d',' -f 2 |sed 's/^\s*//'|cut -d' ' -f1`
    combined_delete=`git --git-dir="$git_repo""/.git" diff --stat $commit1  $commit2 | tail -1 | cut -d',' -f 3 |sed 's/^\s*//'|cut -d' ' -f1`

    echo "               Lines Inserted = $combined_insert"
    echo "               Lines Deleted  = $combined_delete"

    echo -e "\n"

    exit 0
fi

if [ -z $to_date ]
then
    if [ ! -z $from_date ] && [ ! -z $duration ]
    then
        to_date=`date --date "+ $duration days  $from_date " +%F 2>/dev/null`
    else
        to_date=`date "+%F"`
    fi
else
    if [[ $to_date != [0-9][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] ]]
    then
        echo " date format should be yyyy-mm-dd for argument -t"
    exit 1
    fi
fi

if [ ! -z $from_date ]
then
    if [[ $from_date != [0-9][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] ]]
    then
        echo " date format should be yyyy-mm-dd for argument -f"
        exit 1
    fi
fi

if  [ -z $from_date ]  &&  [ -z $duration ]
then
    duration=7  ## taking default duration of 7 days
fi

to_date_int=$(date -d $to_date +"%Y%m%d") > /dev/null

if [ $? -ne 0 ]
then
    echo " date format should be yyyy-mm-dd for argument -t"
    exit 1
fi

if [ -z $from_date ]
then
    from_date=$(date --date "$duration days ago $to_date " +%F)
fi

from_date_int=$(date -d $from_date +"%Y%m%d") > /dev/null

if [ $? -ne 0 ]
then
    echo "date format should be yyyy-mm-dd for argument -f"
    exit 1
fi

if [ $from_date_int -gt $to_date_int ]
then
    echo "date for argument -f should be less that date for argument -t"
    exit 1
fi

if [ -z $git_repo ]
then
    git_repo="."
fi

stat "$git_repo""/.git" 2>&1 1>/dev/null

if [ $? -ne 0 ]
then
    echo "Git repository you have provided is not a valid one"
    show_help
exit 1
fi

if [ ! -z "$authorlist" ]
then

authors=`echo $authorlist | sed 's/,/ /g'`

git_branch=`git --git-dir="$git_repo""/.git" rev-parse --abbrev-ref --symbolic-full-name @{u} 2>/dev/null`

if [ -z $git_branch ]
then
    git_tag=`git --git-dir="$git_repo""/.git" describe --tags  2>/dev/null`
fi

analyse_commit_by_author ## function prints the  details of commits by author in give period

fi

analyse_all_commits

