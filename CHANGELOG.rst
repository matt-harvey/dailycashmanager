Changelog for DailyCashManager
==============================

v1.3.0:

- Default type for new envelopes is now expense, not revenue.
- Fixed bug where budget items were not reset to previous value after
  cancelling and then reopening dialog. (Even though cancelled changes were
  not actually saved, they were still reappearing in the dialog when reopened.)
- Minor changes to user guide.
- Minor code tidy-ups.

v1.2.0:

- Completely new layout for user guide: now on a single page to facilitate
  easy printing and search, with sidebar navigation on wide screens, and
  mobile-friendly layout with expanding / collapsing sections on narrow
  screens.
- In account / envelope editing dialog, enable specific budget items
  to be selected for removal, rather than only the last one.
- In setup wizard, enable specific accounts / envelopes to be selected
  for removal, rather than only the last one.
- Default date range changed from current day to last seven days.
- Warn user if they set the sign (positive or negative) of an account opening
  balance to an abnormal direction for the given account type.
- Ensure account and envelope lists retain their scrolled position after
  a transaction or other event occurs that requires them to be updated.
  (Previously the lists would jump back to the top position each time.)
- Ensure focus always returns to account type control at top left of
  transaction panel, after a transaction is saved, deleted or cancelled.
- Fix issue with Enter key not activating some buttons on Windows.
- Fix "make package" build command not working on some Linux distributions.
- Minor code fixes and tidy-ups.

v1.1.3:

- Resize windows to fit on smaller screens.
- Use Boost for testing instead of UnitTest++.

v1.1.2:

- Fixed bug where one DecimalTextCtrl would revert to its previous value if
  another DecimalTextCtrl in the same EntryGroupCtrl lost focus. Bug appeared on
  (at least) Fedora 19 with Gnome version 3.8.4. Bug did not appear on
  Windows 7.

v1.1.1:

- Fixed bug re. validation of DecimalTextCtrl. On tabbing out of a
  DecimalTextCtrl containing invalid input, and being presented with and then
  dismissing error message, user was unable to focus back into the
  DecimalTextCtrl.
- Fixed bug where error message was displayed twice on invalid inputs in some
  contexts.
- Reset to DateCtrl to its previous content if invalid on unfocus.
- In DateCtrl and DecimalTextCtrl, select entire contents on focus.
- Minor implementation tidy-ups. 

v1.1.0:

- Enabled user to set entity creation date manually in set-up wizard.
- Fixed bug in BudgetPanel, where final budget item could not be removed from
  panel.
- Made it so that in BudgetPanel, when there are no budget item rows to delete,
  the "Remove item" button is disabled.
- Changed slightly the default/suggested Accounts that appear in the setup
  wizard.
- Added descriptions for some of the default/suggested accounts in the setup
  wizard.
- Various fairly minor code tidy-ups.
- Additional unit tests.
- Tidy-ups and improvements in user guide.
- Tidy-ups and improvements in code documentation.

v1.0.0:

- Initial release
