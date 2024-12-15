docker build --build-arg USERNAME=%USERNAME% -t os-course-env .
docker run -dit --name os_course -v "%MOUNTED_DIR%:/home/%USERNAME%/mounted_dir" -e USERNAME=%USERNAME% os-course-env
