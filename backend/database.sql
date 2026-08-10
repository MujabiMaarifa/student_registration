-- Stores student registration records
CREATE TABLE IF NOT EXISTS students (
    student_id    VARCHAR(50)   PRIMARY KEY, -- id: primary key, auto-incremented
    first_name    VARCHAR(100),
    last_name     VARCHAR(100),
    email         VARCHAR(100)  UNIQUE,
    password_hash VARCHAR(255),
    year_of_study INT,
    is_active     BOOLEAN       DEFAULT TRUE,
    created_at    TIMESTAMP     DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS departments (
    dept_code   VARCHAR(10) PRIMARY KEY,
    dept_name   VARCHAR(100) NOT NULL UNIQUE,
    faculty     VARCHAR(100) NOT NULL
);

-- Stores course registered records
CREATE TABLE IF NOT EXISTS courses(
    course_id       VARCHAR(20) PRIMARY KEY,
    dept_code       VARCHAR(10) NOT NULL,
    course_name     VARCHAR(150) NOT NULL,
    description     TEXT,
    capacity        INT NOT NULL,
    enrolled_count  INT NOT NULL DEFAULT 0,
    semester        VARCHAR(20) NOT NULL,
    day_of_week     VARCHAR(20) NOT NULL,
    start_time      TIME NOT NULL,
    end_time        TIME NOT NULL,
    room            VARCHAR(20),
    is_active       BOOLEAN NOT NULL DEFAULT TRUE,
    FOREIGN KEY (dept_code) REFERENCES departments(dept_code) -- student_id: FK referencing students table
);

CREATE TABLE IF NOT EXISTS registrations (
    registration_id SERIAL PRIMARY KEY,
    student_id      VARCHAR(50) NOT NULL,
    course_id       VARCHAR(20) NOT NULL,
    registered_at   TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status          VARCHAR(20) DEFAULT 'active',
    dropped_at      TIMESTAMP NULL,
    FOREIGN KEY (student_id) REFERENCES students(student_id),
    FOREIGN KEY (course_id) REFERENCES courses(course_id),
    UNIQUE (student_id, course_id)
);
