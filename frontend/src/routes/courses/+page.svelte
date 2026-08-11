<script lang="ts">
	import { onMount } from 'svelte';
	import {
		getCourses,
		getRegisteredCourses,
		registerForCourse,
		dropCourse,
		isAuthenticated,
		type Course,
		type Registration
	} from '$lib/api';
	import { goto } from '$app/navigation';

	let courses = $state<Course[]>([]);
	let registeredMap = $state<Record<string, Registration>>({});
	let loading = $state(true);
	let filter = $state('available');
	let error = $state('');

	onMount(async () => {
		if (!isAuthenticated()) {
			goto('/');
			return;
		}
		await loadData();
	});

	async function loadData() {
		loading = true;
		error = '';
		try {
			const [coursesRes, regRes] = await Promise.all([getCourses(), getRegisteredCourses()]);
			const coursesData = await coursesRes.json();
			const regData = await regRes.json();

			if (coursesRes.ok) courses = coursesData;
			else error = coursesData.error || 'Failed to load courses';

			if (regRes.ok) {
				const next: Record<string, Registration> = {};
				for (const r of regData) {
					next[r.course_id] = r;
				}
				registeredMap = next;
			}
		} catch {
			error = 'Could not connect to server';
		} finally {
			loading = false;
		}
	}

	let filteredCourses = $derived.by(() => {
		if (filter === 'available') {
			return courses.filter((c) => !registeredMap[c.course_id]);
		}
		if (filter === 'registered') {
			return courses.filter((c) => registeredMap[c.course_id]?.status === 'active');
		}
		return courses;
	});

	async function handleRegister(courseId: string) {
		const res = await registerForCourse(courseId);
		const data = await res.json();
		if (res.ok) {
			await loadData();
		} else {
			alert(data.error || 'Registration failed');
		}
	}

	async function handleDrop(courseId: string) {
		if (!confirm('Are you sure you want to drop this course?')) return;
		const res = await dropCourse(courseId);
		const data = await res.json();
		if (res.ok) {
			await loadData();
		} else {
			alert(data.error || 'Failed to drop course');
		}
	}

	function statusBadge(status: string) {
		const colors: Record<string, string> = {
			active: 'bg-green-100 text-green-700 dark:bg-green-900/30 dark:text-green-400',
			dropped: 'bg-red-100 text-red-700 dark:bg-red-900/30 dark:text-red-400'
		};
		return colors[status] || '';
	}
</script>

<div class="mx-auto max-w-5xl px-4 py-8">
	<div class="mb-8">
		<h1 class="text-3xl font-bold text-gray-900 dark:text-white">Courses</h1>
		<p class="mt-1 text-sm text-gray-500 dark:text-gray-400">Browse and register for courses</p>
	</div>

	<div class="mb-6 flex flex-wrap items-center gap-3">
		<span class="text-sm font-medium text-gray-600 dark:text-gray-400">Filter by:</span>
		{#each ['available', 'registered'] as f (f)}
			<button
				onclick={() => (filter = f)}
				class="cursor-pointer rounded-full px-4 py-1.5 text-sm font-medium transition-colors {filter ===
				f
					? 'bg-indigo-600 text-white'
					: 'bg-gray-100 text-gray-600 hover:bg-gray-200 dark:bg-white/10 dark:text-gray-400 dark:hover:bg-white/20'}"
			>
				{f.charAt(0).toUpperCase() + f.slice(1)}
			</button>
		{/each}
	</div>

	{#if loading}
		<div class="py-12 text-center text-gray-500 dark:text-gray-400">Loading courses...</div>
	{:else if error}
		<div class="rounded-lg bg-red-50 p-4 text-sm text-red-600 dark:bg-red-900/30 dark:text-red-400">
			{error}
		</div>
	{:else if filteredCourses.length === 0}
		<div class="py-12 text-center text-gray-500 dark:text-gray-400">
			No courses found in this category.
		</div>
	{:else}
		<div class="grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
			{#each filteredCourses as course (course.course_id)}
				<div
					class="rounded-xl border border-gray-200 bg-white p-5 shadow-sm transition-shadow hover:shadow-md dark:border-white/10 dark:bg-gray-900"
				>
					<div class="mb-3 flex items-start justify-between">
						<div>
							<h3 class="font-semibold text-gray-900 dark:text-white">{course.course_name}</h3>
							<p class="text-xs text-gray-500 dark:text-gray-400">{course.course_id}</p>
						</div>
						{#if registeredMap[course.course_id]}
							<span
								class="rounded-full px-2 py-0.5 text-xs font-medium {statusBadge(
									registeredMap[course.course_id].status
								)}"
							>
								{registeredMap[course.course_id].status}
							</span>
						{/if}
					</div>

					<p class="mb-4 line-clamp-2 text-sm text-gray-600 dark:text-gray-400">
						{course.description}
					</p>

					{#if registeredMap[course.course_id]}
						<div class="flex items-center gap-3">
							<a
								href={'/courses/' + course.course_id}
								class="inline-block text-sm font-medium text-indigo-600 hover:text-indigo-500 dark:text-indigo-400"
							>
								See more &rarr;
							</a>
							{#if registeredMap[course.course_id].status === 'active'}
								<button
									onclick={() => handleDrop(course.course_id)}
									class="cursor-pointer rounded-md border border-red-200 px-3 py-1 text-sm font-medium text-red-600 hover:bg-red-50 dark:border-red-900/50 dark:text-red-400 dark:hover:bg-red-900/30"
								>
									Drop
								</button>
							{/if}
						</div>
					{:else}
						<div class="flex items-center gap-2">
							<button
								onclick={() => handleRegister(course.course_id)}
								class="cursor-pointer rounded-lg bg-indigo-600 px-4 py-1.5 text-sm font-medium text-white hover:bg-indigo-700"
							>
								Register
							</button>
							<a
								href={'/courses/' + course.course_id}
								class="text-sm font-medium text-indigo-600 hover:text-indigo-500 dark:text-indigo-400"
							>
								See more &rarr;
							</a>
						</div>
					{/if}
				</div>
			{/each}
		</div>
	{/if}
</div>
