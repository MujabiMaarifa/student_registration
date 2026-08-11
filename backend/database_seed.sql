INSERT INTO departments (dept_code, dept_name, faculty) VALUES
    ('C026', 'Computer Science',             'School of Computing'),
    ('CIC',  'Information Technology',       'School of Computing'),
    ('CSCI', 'Computing Sciences',           'School of Computing'),
    ('CIE',  'Electrical Engineering',       'School of Engineering'),
    ('CIM',  'Mechanical Engineering',       'School of Engineering'),
    ('CIV',  'Civil Engineering',            'School of Engineering'),
    ('CBA',  'Business Administration',      'School of Business'),
    ('CEC',  'Economics',                    'School of Business'),
    ('MAT',  'Mathematics',                  'School of Science'),
    ('BIO',  'Biochemistry',                 'School of Science'),
    ('MSC',  'Medicine',                     'School of Medicine'),
    ('C033', 'Statistics',                   'School of Science')
ON CONFLICT (dept_code) DO NOTHING;
