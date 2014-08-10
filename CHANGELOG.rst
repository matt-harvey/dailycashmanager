Changelog for DailyCashManager
==============================

v1.1.2:

- Fixed bug, which appeared at least on Fedora, but not on Windows, where a
  DecimalTextCtrl would sometimes unexpectedly revert to its previous value.

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
