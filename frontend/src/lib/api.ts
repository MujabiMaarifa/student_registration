export interface Course {
	course_id: string;
	dept_code: string;
	course_name: string;
	description: string;
	capacity: number;
	enrolled_count: number;
	semester: string;
	day_of_week: string;
	start_time: string;
	end_time: string;
	room: string;
	lecturer_id?: string;
	is_active: boolean;
}

export interface Registration {
	course_id: string;
	dept_code: string;
	course_name: string;
	description: string;
	capacity: number;
	enrolled_count: number;
	semester: string;
	day_of_week: string;
	start_time: string;
	end_time: string;
	room: string;
	is_active: boolean;
	registered_at: string;
	status: string;
}

const API_BASE = 'http://localhost:1234';

function getToken(): string | null {
	if (typeof localStorage === 'undefined') return null;
	return localStorage.getItem('token');
}

export function getStudentId(): string | null {
	if (typeof localStorage === 'undefined') return null;
	return localStorage.getItem('user_id') || localStorage.getItem('student_id');
}

export function getRole(): string | null {
	if (typeof localStorage === 'undefined') return null;
	return localStorage.getItem('role');
}

export function setAuth(token: string, userId: string, role: string) {
	localStorage.setItem('token', token);
	localStorage.setItem('user_id', userId);
	localStorage.setItem('role', role);
}

export function clearAuth() {
	localStorage.removeItem('token');
	localStorage.removeItem('user_id');
	localStorage.removeItem('student_id');
	localStorage.removeItem('role');
}

export function isAuthenticated(): boolean {
	return getToken() !== null;
}

async function request(path: string, options: RequestInit = {}): Promise<Response> {
	const token = getToken();
	const headers: Record<string, string> = {
		'Content-Type': 'application/json',
		...(options.headers as Record<string, string>)
	};
	if (token) {
		headers['Authorization'] = `Bearer ${token}`;
	}
	return fetch(`${API_BASE}${path}`, { ...options, headers });
}

export async function login(email: string, password: string) {
	return request('/login', {
		method: 'POST',
		body: JSON.stringify({ email, password })
	});
}

export async function registerStudent(data: {
	student_id: string;
	first_name: string;
	last_name: string;
	email: string;
	password: string;
	year_of_study: number;
	dept_code: string;
}) {
	return request('/register', {
		method: 'POST',
		body: JSON.stringify(data)
	});
}

export async function registerLecturer(data: {
	lecturer_id: string;
	first_name: string;
	last_name: string;
	email: string;
	password: string;
	dept_code: string;
}) {
	return request('/lecturer/register', {
		method: 'POST',
		body: JSON.stringify(data)
	});
}

export async function createCourse(data: {
	course_id: string;
	course_name: string;
	description?: string;
	capacity: number;
	semester: string;
	day_of_week: string;
	start_time: string;
	end_time: string;
	room?: string;
}) {
	return request('/courses', {
		method: 'POST',
		body: JSON.stringify(data)
	});
}

export async function getLecturerCourses() {
	return request('/lecturer/courses');
}

export async function getCourses() {
	return request('/courses');
}

export async function getCourse(courseId: string) {
	return request(`/courses/${courseId}`);
}

export async function registerForCourse(courseId: string) {
	return request(`/courses/${courseId}/register`, {
		method: 'POST'
	});
}

export async function dropCourse(courseId: string) {
	return request(`/courses/${courseId}/drop`, {
		method: 'POST'
	});
}

export async function getRegisteredCourses() {
	return request('/courses/registered');
}
