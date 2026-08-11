CREATE TABLE IF NOT EXISTS departments (
    dept_code   VARCHAR(10) PRIMARY KEY,
    dept_name   VARCHAR(100) NOT NULL UNIQUE,
    faculty     VARCHAR(100) NOT NULL
);

-- Stores student registration records
CREATE TABLE IF NOT EXISTS students (
    student_id    VARCHAR(50)   PRIMARY KEY, -- id: primary key, auto-incremented
    first_name    VARCHAR(100),
    last_name     VARCHAR(100),
    email         VARCHAR(100)  UNIQUE,
    password_hash VARCHAR(255),
    year_of_study INT,
    dept_code     VARCHAR(10)   NOT NULL,
    is_active     BOOLEAN       DEFAULT TRUE,
    created_at    TIMESTAMP     DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (dept_code) REFERENCES departments(dept_code)
);

-- Stores lecturer records
CREATE TABLE IF NOT EXISTS lecturers (
    lecturer_id   VARCHAR(50) PRIMARY KEY,
    first_name    VARCHAR(100),
    last_name     VARCHAR(100),
    email         VARCHAR(100) UNIQUE,
    password_hash VARCHAR(255),
    dept_code     VARCHAR(10) NOT NULL,
    is_active     BOOLEAN DEFAULT TRUE,
    created_at    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (dept_code) REFERENCES departments(dept_code)
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
    lecturer_id     VARCHAR(50),
    is_active       BOOLEAN NOT NULL DEFAULT TRUE,
    FOREIGN KEY (dept_code) REFERENCES departments(dept_code),
    FOREIGN KEY (lecturer_id) REFERENCES lecturers(lecturer_id)
);

-- Adds the lecturer_id column to existing installations
ALTER TABLE courses ADD COLUMN IF NOT EXISTS lecturer_id VARCHAR(50);

-- Adds the dept_code column to existing installations
ALTER TABLE students ADD COLUMN IF NOT EXISTS dept_code VARCHAR(10);
-- Backfill dept_code for existing students from their student_id prefix (e.g. C026-...)
UPDATE students
SET dept_code = split_part(student_id, '-', 1)
WHERE dept_code IS NULL
  AND split_part(student_id, '-', 1) IN (SELECT dept_code FROM departments);
-- Enforce that dept_code references an existing department
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM pg_constraint WHERE conname = 'students_dept_code_fkey') THEN
        ALTER TABLE students ADD CONSTRAINT students_dept_code_fkey
            FOREIGN KEY (dept_code) REFERENCES departments(dept_code);
    END IF;
END $$;

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
