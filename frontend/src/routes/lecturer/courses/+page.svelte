<script lang="ts">
	import { onMount } from 'svelte';
	import {
		getLecturerCourses,
		createCourse,
		getRole,
		isAuthenticated,
		type Course
	} from '$lib/api';
	import { goto } from '$app/navigation';

	let courses = $state<Course[]>([]);
	let loading = $state(true);
	let error = $state('');
	let saving = $state(false);
	let formError = $state('');
	let success = $state('');

	let course_id = $state('');
	let course_name = $state('');
	let description = $state('');
	let capacity = $state(100);
	let semester = $state('');
	let day_of_week = $state('Monday');
	let start_time = $state('09:00');
	let end_time = $state('11:00');
	let room = $state('');

	const days = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday'];

	onMount(async () => {
		if (!isAuthenticated() || getRole() !== 'lecturer') {
			goto('/');
			return;
		}
		await loadCourses();
	});

	async function loadCourses() {
		loading = true;
		error = '';
		try {
			const res = await getLecturerCourses();
			const data = await res.json();
			if (res.ok) {
				courses = data;
			} else {
				error = data.error || 'Failed to load courses';
			}
		} catch {
			error = 'Could not connect to server';
		} finally {
			loading = false;
		}
	}

	async function handleCreate(e: Event) {
		e.preventDefault();
		formError = '';
		success = '';
		saving = true;
		try {
			const res = await createCourse({
				course_id,
				course_name,
				description,
				capacity,
				semester,
				day_of_week,
				start_time,
				end_time,
				room
			});
			const data = await res.json();
			if (!res.ok) {
				formError = data.error || 'Failed to create course';
				return;
			}
			success = `Course ${data.course_id} created successfully`;
			course_id = '';
			course_name = '';
			description = '';
			capacity = 100;
			semester = '';
			start_time = '09:00';
			end_time = '11:00';
			room = '';
			await loadCourses();
		} catch {
			formError = 'Could not connect to server';
		} finally {
			saving = false;
		}
	}
</script>

<div class="mx-auto max-w-5xl px-4 py-8">
	<div class="mb-8">
		<h1 class="text-3xl font-bold text-gray-900 dark:text-white">My Courses</h1>
		<p class="mt-1 text-sm text-gray-500 dark:text-gray-400">
			Create and manage the courses students will register for
		</p>
	</div>

	<div
		class="mb-8 rounded-2xl border border-gray-200 bg-white p-6 shadow-sm dark:border-white/10 dark:bg-gray-900"
	>
		<h2 class="mb-4 text-lg font-semibold text-gray-900 dark:text-white">Create a new course</h2>
		<form onsubmit={handleCreate} class="grid gap-4 sm:grid-cols-2">
			<div>
				<label for="course_id" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Course ID</label
				>
				<input
					id="course_id"
					type="text"
					bind:value={course_id}
					placeholder="e.g. CSCI201"
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>
			<div>
				<label for="course_name" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Course Name</label
				>
				<input
					id="course_name"
					type="text"
					bind:value={course_name}
					placeholder="e.g. Data Structures"
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>
			<div class="sm:col-span-2">
				<label for="description" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Description</label
				>
				<textarea
					id="description"
					bind:value={description}
					rows="3"
					placeholder="Brief description of the course"
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				></textarea>
			</div>
			<div>
				<label for="semester" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Semester</label
				>
				<input
					id="semester"
					type="text"
					bind:value={semester}
					placeholder="e.g. Fall 2026"
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>
			<div>
				<label for="capacity" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Capacity</label
				>
				<input
					id="capacity"
					type="number"
					min="1"
					bind:value={capacity}
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>
			<div>
				<label for="day_of_week" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Day of Week</label
				>
				<select
					id="day_of_week"
					bind:value={day_of_week}
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				>
					{#each days as day (day)}
						<option value={day}>{day}</option>
					{/each}
				</select>
			</div>
			<div>
				<label for="start_time" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Start Time</label
				>
				<input
					id="start_time"
					type="time"
					bind:value={start_time}
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>
			<div>
				<label for="end_time" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>End Time</label
				>
				<input
					id="end_time"
					type="time"
					bind:value={end_time}
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>
			<div>
				<label for="room" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Room</label
				>
				<input
					id="room"
					type="text"
					bind:value={room}
					placeholder="e.g. B204"
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>

			{#if formError}
				<div
					class="rounded-lg bg-red-50 p-3 text-sm text-red-600 sm:col-span-2 dark:bg-red-900/30 dark:text-red-400"
				>
					{formError}
				</div>
			{/if}
			{#if success}
				<div
					class="rounded-lg bg-green-50 p-3 text-sm text-green-700 sm:col-span-2 dark:bg-green-900/30 dark:text-green-400"
				>
					{success}
				</div>
			{/if}

			<button
				type="submit"
				disabled={saving}
				class="rounded-lg bg-indigo-600 px-6 py-2.5 text-sm font-semibold text-white hover:bg-indigo-700 disabled:cursor-not-allowed disabled:opacity-50 sm:col-span-2"
			>
				{saving ? 'Creating...' : 'Create Course'}
			</button>
		</form>
	</div>

	<h2 class="mb-4 text-lg font-semibold text-gray-900 dark:text-white">Your courses</h2>

	{#if loading}
		<div class="py-12 text-center text-gray-500 dark:text-gray-400">Loading courses...</div>
	{:else if error}
		<div class="rounded-lg bg-red-50 p-4 text-sm text-red-600 dark:bg-red-900/30 dark:text-red-400">
			{error}
		</div>
	{:else if courses.length === 0}
		<div class="py-12 text-center text-gray-500 dark:text-gray-400">
			You have not created any courses yet.
		</div>
	{:else}
		<div class="grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
			{#each courses as course (course.course_id)}
				<div
					class="rounded-xl border border-gray-200 bg-white p-5 shadow-sm transition-shadow hover:shadow-md dark:border-white/10 dark:bg-gray-900"
				>
					<div class="mb-3">
						<h3 class="font-semibold text-gray-900 dark:text-white">{course.course_name}</h3>
						<p class="text-xs text-gray-500 dark:text-gray-400">{course.course_id}</p>
					</div>
					<p class="mb-4 line-clamp-2 text-sm text-gray-600 dark:text-gray-400">
						{course.description}
					</p>
					<div class="text-sm text-gray-500 dark:text-gray-400">
						<div>{course.day_of_week} {course.start_time} - {course.end_time}</div>
						<div>{course.semester}</div>
						<div class="mt-1">
							<span class="font-medium text-gray-900 dark:text-white">{course.enrolled_count}</span>
							/ {course.capacity} enrolled
						</div>
					</div>
				</div>
			{/each}
		</div>
	{/if}
</div>
