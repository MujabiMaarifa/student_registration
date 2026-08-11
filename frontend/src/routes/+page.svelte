<script lang="ts">
	import { login, setAuth } from '$lib/api';
	import { goto } from '$app/navigation';

	let email = $state('');
	let password = $state('');
	let error = $state('');
	let loading = $state(false);

	function decodeJwtPayload(token: string): Record<string, unknown> {
		try {
			const parts = token.split('.');
			if (parts.length !== 3) return {};
			const decoded = atob(parts[1].replace(/-/g, '+').replace(/_/g, '/'));
			return JSON.parse(decoded);
		} catch {
			return {};
		}
	}

	async function handleLogin(e: Event) {
		e.preventDefault();
		error = '';
		loading = true;
		try {
			const res = await login(email, password);
			const data = await res.json();
			if (!res.ok) {
				error = data.error || 'Login failed';
				return;
			}
			const payload = decodeJwtPayload(data.token);
			const role = data.role || 'student';
			const userId =
				(role === 'lecturer' ? (payload.lecturer_id as string) : (payload.student_id as string)) ||
				email;
			setAuth(data.token, userId, role);
			goto(role === 'lecturer' ? '/lecturer/courses' : '/courses');
		} catch {
			error = 'Could not connect to server';
		} finally {
			loading = false;
		}
	}
</script>

<div class="flex min-h-screen items-center justify-center bg-gray-50 px-4 dark:bg-gray-950">
	<div class="w-full max-w-md rounded-2xl bg-white p-8 shadow-lg dark:bg-gray-900">
		<div class="mb-8 text-center">
			<div
				class="mx-auto mb-4 flex h-16 w-16 items-center justify-center rounded-full bg-indigo-100 text-2xl dark:bg-indigo-900"
			>
				🎓
			</div>
			<h1 class="text-2xl font-bold text-gray-900 dark:text-white">Course Registration Platform</h1>
			<p class="mt-1 text-sm text-gray-500 dark:text-gray-400">Welcome back! Please sign in.</p>
		</div>

		<form onsubmit={handleLogin} class="space-y-5">
			<div>
				<label for="email" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Email</label
				>
				<input
					id="email"
					type="email"
					bind:value={email}
					placeholder="your@email.com"
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm text-gray-900 placeholder:text-gray-400 focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white dark:placeholder:text-gray-500"
				/>
			</div>

			<div>
				<label for="password" class="block text-sm font-medium text-gray-700 dark:text-gray-300"
					>Password</label
				>
				<input
					id="password"
					type="password"
					bind:value={password}
					placeholder="Enter your password"
					required
					class="mt-1 block w-full rounded-lg border border-gray-300 bg-white px-4 py-2.5 text-sm text-gray-900 placeholder:text-gray-400 focus:border-indigo-500 focus:ring-2 focus:ring-indigo-500/20 dark:border-white/10 dark:bg-white/5 dark:text-white dark:placeholder:text-gray-500"
				/>
			</div>

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
				{loading ? 'Signing in...' : 'Login'}
			</button>
		</form>

		<p class="mt-6 text-center text-sm text-gray-500 dark:text-gray-400">
			Don't have an account?
			<a
				href="/register"
				class="font-medium text-indigo-600 hover:text-indigo-500 dark:text-indigo-400">Sign up</a
			>
		</p>
	</div>
</div>
