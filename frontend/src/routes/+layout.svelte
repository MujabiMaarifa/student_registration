<script lang="ts">
	import './layout.css';
	import favicon from '$lib/assets/favicon.svg';
	import { isAuthenticated, clearAuth, getStudentId, getRole } from '$lib/api';
	import { page } from '$app/state';

	let { children } = $props();

	let role = $derived(getRole());

	function logout() {
		clearAuth();
		window.location.href = '/';
	}
</script>

<svelte:head><link rel="icon" href={favicon} /></svelte:head>

{#if isAuthenticated() && page.url.pathname !== '/'}
	<nav
		class="flex items-center justify-between border-b border-gray-200 bg-white px-6 py-3 dark:border-white/10 dark:bg-gray-900"
	>
		<div class="flex items-center gap-6">
			<a
				href={role === 'lecturer' ? '/lecturer/courses' : '/courses'}
				class="text-xl font-bold tracking-tight text-indigo-600 dark:text-indigo-400">LOGO</a
			>
			{#if role === 'lecturer'}
				<a
					href="/lecturer/courses"
					class="text-sm font-medium text-gray-700 hover:text-indigo-600 dark:text-gray-300 dark:hover:text-indigo-400"
					>MY COURSES</a
				>
			{:else}
				<a
					href="/courses"
					class="text-sm font-medium text-gray-700 hover:text-indigo-600 dark:text-gray-300 dark:hover:text-indigo-400"
					>COURSES</a
				>
			{/if}
		</div>
		<div class="flex items-center gap-4">
			<span class="text-sm text-gray-600 dark:text-gray-400">
				{getStudentId()}{#if role === 'lecturer'}
					(lecturer){/if}
			</span>
			<button
				onclick={logout}
				class="cursor-pointer rounded-md bg-red-500 px-3 py-1 text-sm text-white hover:bg-red-600"
				>Logout</button
			>
		</div>
	</nav>
{/if}

<main>
	{@render children()}
</main>
