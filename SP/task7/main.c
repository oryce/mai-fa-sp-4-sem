#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void print_file_info(const int dir_fd, const char *filename) {
	struct stat file_stat;
	if (fstatat(dir_fd, filename, &file_stat, AT_SYMLINK_NOFOLLOW) < 0) {
		perror("fstatat");
		return;
	}

	char type;
	if (S_ISREG(file_stat.st_mode))
		type = '-';  // Regular file
	else if (S_ISDIR(file_stat.st_mode))
		type = 'd';  // Directory
	else if (S_ISLNK(file_stat.st_mode))
		type = 'l';  // Link
	else if (S_ISCHR(file_stat.st_mode))
		type = 'c';  // Character device
	else if (S_ISBLK(file_stat.st_mode))
		type = 'b';  // Block device
	else if (S_ISFIFO(file_stat.st_mode))
		type = 'p';  // FIFO (named pipe)
	else if (S_ISSOCK(file_stat.st_mode))
		type = 's';  // Socket
	else
		type = '?';

	char permissions[12];
	snprintf(permissions, sizeof(permissions), "%c %c%c%c%c%c%c%c%c%c", type,
	         (file_stat.st_mode & S_IRUSR) ? 'r' : '-', (file_stat.st_mode & S_IWUSR) ? 'w' : '-',
	         (file_stat.st_mode & S_IXUSR) ? 'x' : '-', (file_stat.st_mode & S_IRGRP) ? 'r' : '-',
	         (file_stat.st_mode & S_IWGRP) ? 'w' : '-', (file_stat.st_mode & S_IXGRP) ? 'x' : '-',
	         (file_stat.st_mode & S_IROTH) ? 'r' : '-', (file_stat.st_mode & S_IWOTH) ? 'w' : '-',
	         (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

	const struct passwd *pwd = getpwuid(file_stat.st_uid);
	const struct group *grp = getgrgid(file_stat.st_gid);

	char time_buf[64];
	const struct tm *tm_info = localtime(&file_stat.st_mtime);
	strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", tm_info);

	printf("%s %2lu %-8s %-8s %8lld %s %s\n", permissions, file_stat.st_nlink,
	       pwd ? pwd->pw_name : "unknown", grp ? grp->gr_name : "unknown",
	       (long long)file_stat.st_size, time_buf, filename);

	printf("  First disk address: %llu\n", (unsigned long long)file_stat.st_ino);
}

void list_directory(const char *dirname) {
	DIR *dir;
	struct dirent *entry;

	printf("%s:\n", dirname);

	if ((dir = opendir(dirname)) == NULL) {
		perror("opendir");
		return;
	}

	const int dir_fd = dirfd(dir);
	if (dir_fd == -1) {
		perror("dirfd");
		closedir(dir);
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

		print_file_info(dir_fd, entry->d_name);
	}

	closedir(dir);
	printf("\n");
}

int main(const int argc, char **argv) {
	if (argc < 2) {
		list_directory(".");
	} else {
		for (int i = 1; i < argc; i++) {
			list_directory(argv[i]);
		}
	}
	return 0;
}