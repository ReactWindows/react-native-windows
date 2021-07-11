/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

test('Verify presence of NewAppScreen Header', async () => {
  const header = await $('new-app-screen-header');
  expect(await header.isDisplayed()).toBe(true);
});
