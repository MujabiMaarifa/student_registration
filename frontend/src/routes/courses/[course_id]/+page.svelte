<script lang="ts">
	import { onMount } from 'svelte';
	import { page } from '$app/state';
	import {
		getCourse,
		getRegisteredCourses,
		registerForCourse,
		dropCourse,
		isAuthenticated,
		type Course,
		type Registration
	} from '$lib/api';
	import { goto } from '$app/navigation';

	let course_id: string | undefined = $derived(page.params.course_id);
	let course = $state<Course | null>(null);
	let registration = $state<Registration | null>(null);
	let loading = $state(true);
	let error = $state('');

	onMount(async () => {
		if (!isAuthenticated()) {
			goto('/');
			return;
		}
		if (!course_id) {
			error = 'Invalid course ID';
			loading = false;
			return;
		}
		await loadData();
	});

	async function loadData() {
		if (!course_id) return;
		loading = true;
		error = '';
		try {
			const [courseRes, regRes] = await Promise.all([getCourse(course_id), getRegisteredCourses()]);
			const courseData = await courseRes.json();
			const regData = await regRes.json();

			if (courseRes.ok) {
				course = courseData;
			} else {
				error = courseData.error || 'Course not found';
			}

			if (regRes.ok) {
				registration = regData.find((r: Registration) => r.course_id === course_id) || null;
			}
		} catch {
			error = 'Could not connect to server';
		} finally {
			loading = false;
		}
	}

	async function handleRegister() {
		if (!course_id) return;
		const res = await registerForCourse(course_id);
		const data = await res.json();
		if (res.ok) {
			await loadData();
		} else {
			alert(data.error || 'Registration failed');
		}
	}

	async function handleDrop() {
		if (!course_id) return;
		if (!confirm('Are you sure you want to drop this course?')) return;
		const res = await dropCourse(course_id);
		const data = await res.json();
		if (res.ok) {
			await loadData();
		} else {
			alert(data.error || 'Failed to drop course');
		}
	}
</script>

<div class="mx-auto max-w-4xl px-4 py-8">
	<a
		href="/courses"
		class="mb-6 inline-block text-sm text-indigo-600 hover:text-indigo-500 dark:text-indigo-400"
		>&larr; Back to courses</a
	>

	{#if loading}
		<div class="py-12 text-center text-gray-500 dark:text-gray-400">Loading course details...</div>
	{:else if error}
		<div class="rounded-lg bg-red-50 p-4 text-sm text-red-600 dark:bg-red-900/30 dark:text-red-400">
			{error}
		</div>
	{:else if course}
		<div
			class="mb-8 rounded-2xl border border-gray-200 bg-white p-8 shadow-sm dark:border-white/10 dark:bg-gray-900"
		>
			<div class="mb-4 flex items-start justify-between">
				<div>
					<h1 class="text-3xl font-bold text-gray-900 dark:text-white">{course.course_name}</h1>
					<p class="mt-1 text-sm text-gray-500 dark:text-gray-400">{course.course_id}</p>
				</div>
				{#if registration}
					<span
						class="rounded-full bg-green-100 px-3 py-1 text-sm font-medium text-green-700 dark:bg-green-900/30 dark:text-green-400"
					>
						{registration.status}
					</span>
				{/if}
			</div>

			<p class="mb-6 text-gray-600 dark:text-gray-400">{course.description}</p>

			<div class="mb-6 grid grid-cols-2 gap-4 text-sm">
				<div>
					<span class="text-gray-500 dark:text-gray-400">Department:</span>
					<span class="ml-1 font-medium text-gray-900 dark:text-white">{course.dept_code}</span>
				</div>
				<div>
					<span class="text-gray-500 dark:text-gray-400">Semester:</span>
					<span class="ml-1 font-medium text-gray-900 dark:text-white">{course.semester}</span>
				</div>
				<div>
					<span class="text-gray-500 dark:text-gray-400">Schedule:</span>
					<span class="ml-1 font-medium text-gray-900 dark:text-white"
						>{course.day_of_week} {course.start_time} - {course.end_time}</span
					>
				</div>
				<div>
					<span class="text-gray-500 dark:text-gray-400">Room:</span>
					<span class="ml-1 font-medium text-gray-900 dark:text-white">{course.room}</span>
				</div>
				<div>
					<span class="text-gray-500 dark:text-gray-400">Capacity:</span>
					<span class="ml-1 font-medium text-gray-900 dark:text-white"
						>{course.enrolled_count}/{course.capacity}</span
					>
				</div>
			</div>

			{#if registration && registration.status === 'active'}
				<button
					onclick={handleDrop}
					class="rounded-lg bg-red-600 px-6 py-2.5 text-sm font-semibold text-white hover:bg-red-700"
				>
					Drop course
				</button>
			{:else if !registration}
				<button
					onclick={handleRegister}
					class="rounded-lg bg-indigo-600 px-6 py-2.5 text-sm font-semibold text-white hover:bg-indigo-700"
				>
					Register for this course
				</button>
			{/if}
		</div>
	{/if}
</div>
