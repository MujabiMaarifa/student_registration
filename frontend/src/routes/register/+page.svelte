<script lang="ts">
	import { registerStudent, registerLecturer } from '$lib/api';
	import { goto } from '$app/navigation';

	let role = $state<'student' | 'lecturer'>('student');
	let student_id = $state('');
	let lecturer_id = $state('');
	let first_name = $state('');
	let last_name = $state('');
	let email = $state('');
	let password = $state('');
	let year_of_study = $state(1);
	let dept_code = $state('');
	let error = $state('');
	let loading = $state(false);

	async function handleRegister(e: Event) {
		e.preventDefault();
		error = '';
		loading = true;
		try {
			const res =
				role === 'lecturer'
					? await registerLecturer({
							lecturer_id,
							first_name,
							last_name,
							email,
							password,
							dept_code
						})
					: await registerStudent({
							student_id,
							first_name,
							last_name,
							email,
							password,
							year_of_study,
							dept_code
						});
			const data = await res.json();
			if (!res.ok) {
				error = data.error || 'Registration failed';
				return;
			}
			goto('/?registered=true');
		} catch {
			error = 'Could not connect to server';
		} finally {
			loading = false;
		}
	}
</script>

<div class="flex min-h-screen items-center justify-center bg-gray-50 px-4 py-12 dark:bg-gray-950">
	<div class="w-full max-w-md rounded-2xl bg-white p-8 shadow-lg dark:bg-gray-900">
		<div class="mb-8 text-center">
			<h1 class="text-2xl font-bold text-gray-900 dark:text-white">Create Account</h1>
			<p class="mt-1 text-sm text-gray-500 dark:text-gray-400">
				Register as a {role === 'lecturer' ? 'lecturer' : 'student'}
			</p>
		</div>

		<div class="mb-6 grid grid-cols-2 gap-2 rounded-lg bg-gray-100 p-1 dark:bg-white/5">
			<button
				type="button"
				onclick={() => (role = 'student')}
				class="cursor-pointer rounded-md px-4 py-2 text-sm font-medium transition-colors {role ===
				'student'
					? 'bg-white text-gray-900 shadow dark:bg-white/10 dark:text-white'
					: 'text-gray-600 hover:text-gray-900 dark:text-gray-400'}"
			>
				Student
			</button>
			<button
				type="button"
				onclick={() => (role = 'lecturer')}
				class="cursor-pointer rounded-md px-4 py-2 text-sm font-medium transition-colors {role ===
				'lecturer'
					? 'bg-white text-gray-900 shadow dark:bg-white/10 dark:text-white'
					: 'text-gray-600 hover:text-gray-900 dark:text-gray-400'}"
			>
				Lecturer
			</button>
		</div>

		<form onsubmit={handleRegister} class="space-y-4">
			<div>
				<label for="user_id" class="block text-sm font-medium text-gray-700 dark:text-gray-300">
					{role === 'lecturer' ? 'Lecturer ID' : 'Student ID'}
				</label>
				{#if role === 'lecturer'}
					<input
						id="user_id"
						type="text"
						bind:value={lecturer_id}
						required
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					/>
				{:else}
					<input
						id="user_id"
						type="text"
						bind:value={student_id}
						required
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					/>
				{/if}
			</div>

			<div class="grid grid-cols-2 gap-4">
				<div>
					<label for="first_name" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
						>First Name</label
					>
					<input
						id="first_name"
						type="text"
						bind:value={first_name}
						required
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					/>
				</div>
				<div>
					<label for="last_name" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
						>Last Name</label
					>
					<input
						id="last_name"
						type="text"
						bind:value={last_name}
						required
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					/>
				</div>
			</div>

			<div>
				<label for="reg_email" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Email</label
				>
				<input
					id="reg_email"
					type="email"
					bind:value={email}
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>

			<div>
				<label for="reg_password" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Password</label
				>
				<input
					id="reg_password"
					type="password"
					bind:value={password}
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
				/>
			</div>

			{#if role === 'student'}
				<div>
					<label for="dept_code" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
						>Department Code</label
					>
					<input
						id="dept_code"
						type="text"
						bind:value={dept_code}
						placeholder="e.g. C026"
						required
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					/>
				</div>
				<div>
					<label
						for="year_of_study"
						class="block text-sm font-medium text-gray-700 dark:text-gray-300">Year of Study</label
					>
					<select
						id="year_of_study"
						bind:value={year_of_study}
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					>
						{#each [1, 2, 3, 4, 5, 6] as year (year)}
							<option value={year}>Year {year}</option>
						{/each}
					</select>
				</div>
			{:else}
				<div>
					<label for="dept_code" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
						>Department Code</label
					>
					<input
						id="dept_code"
						type="text"
						bind:value={dept_code}
						placeholder="e.g. CSCI"
						required
						class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white"
					/>
				</div>
			{/if}

			{#if error}
				<div
					class="rounded-lg bg-red-50 p-3 text-sm text-red-600 dark:bg-red-900/30 dark:text-red-400"
				>
					{error}
				</div>
			{/if}

			<button
				type="submit"
				disabled={loading}
				class="w-full rounded-lg bg-indigo-600 px-4 py-2.5 text-sm font-semibold text-white hover:bg-indigo-700 disabled:cursor-not-allowed disabled:opacity-50"
			>
				{loading ? 'Registering...' : 'Create Account'}
			</button>
		</form>

		<p class="mt-4 text-center text-sm text-gray-500 dark:text-gray-400">
			Already have an account?
			<a href="/" class="font-medium text-indigo-600 hover:text-indigo-500 dark:text-indigo-400"
				>Sign in</a
			>
		</p>
	</div>
</div>
