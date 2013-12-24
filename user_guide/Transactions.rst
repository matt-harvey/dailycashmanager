Transactions
============

Every time you perform any transaction - such as spending money, receiving a
salary payment, withdrawing money from the bank, transferring funds between
bank accounts, or paying a bill - you should record the transaction in your
DCM file. 

Entering a one-off transaction
------------------------------

Most of the time you will be entering a single transaction. This is a matter
of entering the transaction details in the transaction panel at the top right
of the main window (*not* the *Transactions* tab at the top left).

This is best illustrated using examples.

Example 1 - you spend 30.95 from your credit card on a meal at a restaurant:

- Select *Spend* from the transaction type box at the top left of the
  transaction panel.
- Enter "30.95" into the amount box at the top right of the transaction panel.
- In the next row, select your credit card account from box labelled *Account*.
  Enter an optional memo in the row next to the *Account* box. For example, you
  might enter the name of the restaurant.
- Select "Food" from the box labelled *Envelope*. Enter an optional memo, e.g.
  "dinner at restaurant".
- In the next row, there should be a box containing the text, "Record once only,
  on". This means you are entering a single, one-off transaction. Leave this
  as is.
- There should be a box near the bottom right of the transaction panel,
  containing today's date. If you are entering the transaction on the same day
  on which the transaction occurred (generally a good idea), you can
  leave this as is; otherwise, manually enter the date the transaction actually
  occurred in this box.
- At the bottom right of the transaction panel, click *Save* to confirm and save
  the transaction; or click *Clear* if you want to abandon this transaction
  entry and start again.

Example 2 - you earn 2000.00 in wages, deposited into your "XYZ Bank savings"
account:

- Select *Earn* from the transaction type box at the top left of the transaction
  panel.
- Enter "2000" into the amount box at the top right of the transaction panel.
- In the next row, select "XYZ Bank savings" from the box labelled
  *Account*. Enter an optional memo in the row next to the *Account* box. For
  example, you might enter the name of your employer.
- In the next row, select "Salary" from the box labelled *Envelope*. Enter an
  optional memo: you might enter the time period that this pay was for, for
  example; or you might simply leave this blank.
- In the next row, there should be a box containing the text, "Record once only,
  on". This means you are entering a single, one-off transaction. Leave this
  as is.
- There should be a box near the bottom right of the transaction panel,
  containing today's date. If you are entering the transaction on the same
  day on which the transaction occurred (generally a good idea), you can leave
  this as is; otherwise, manually enter the date the transaction actually
  occurred in this box.
- At the bottom right of the transaction panel, click *Save* to confirm and save
  the transaction; or click *Clear* if you want to abandon this transaction
  entry and start again.

Example 3 - you withdraw 100.00 in cash from your "XYZ Bank savings" account:

- Select *Account transfer* from the transaction type box at the top left of the
  transaction panel.
- Enter "100" into tha amount box at the top right of the transaction panel.
- In the next row, select "XYZ Bank savings" from the box labelled *Source*.
  Enter an optional memo in the row next to the *Source* box (for example, you
  might enter "withdraw cash at bank ATM").
- In the next row, select "Cash" from the box labelled *Destination*. Enter an
  optional memo.
- Select "Record once only, on", enter the date and click *Save*.

Note that when entering the date of a one-off transaction, you can enter a past
date, leave it as today's date, or enter a future date. Regardless of what date
you enter (even if you enter a future date), the corresponding account and
envelope balances will be update immediately. (This is unlike a recurring
transaction, which is not actually recorded until the "Next date" is reached -
see `below`_.)

Generic transactions
....................

In addition to the transaction types *Spend*, *Earn* and *Account transfer*,
there is a fourth transaction type, *Generic*. You might never feel the need
to use this category; and if you don't that's fine. It is provided to enable
you to enter complicated transactions that don't fit neatly into any of the
other categories. When you select the *Generic* transaction type, you will
notice that the account and envelope selection boxes are labelled *CR* and *DR*.
These refer to the concept of accounting credits and debits. You will also
notice that both for the *CR* and the *DR* row, you can select *any* account
or envelope (other than pure envelopes) in either row. You can think of the *CR*
row is the equivalent of the "source" row - and the *DR* row as the
"destination" row. An example will help to clarify.

Suppose you withdraw 200.00 from your "XYZ Bank savings" account using an
automatic teller machine (ATM), and in the process of doing so, you incur a 2.00
transaction fee. This transaction has characteristics both of an *Account
transfer* transaction (since you are transferring money from your bank account
to the "Cash" account) and of a *Spend* transaction (since you are spending 2.00
on the transaction fee). If you wanted to, you could break this down into two
separate transactions, and enter these separately (the 200.00 transfer, and the
2.00 expenditure on bank fees). However, there is faster way of entering this
transaction, using the *Generic* transaction type, as follows:

- Select *Generic* from the transaction type box.
- Enter "202" in the amount box at the top right. This corresponds to the
  total amount deducted from your bank account in the course of this
  transaction.
- In the *CR* box, select "XYZ Bank savings".
- In the second row, select "Cash", then click the *Split* button to the
  right (see `Split transactions`_ for more explanation on this).
- There will now be two rows in the bottom half of the transaction control,
  both with "Cash" selected in the *DR* control. In the first of these
  rows, enter "200" in the amount box, representing the 200.00 cash you
  physically withdrew from the ATM. In the very bottom row, select "Bank fees"
  from the *DR* box, and enter "2" in the amount box to the right. If you can
  like you can enter text into the "Memo" boxes. E.g., you might enter "ATM fee"
  in the very bottom row, and "ATM withdrawal" in the middle row.
- Enter the date and click *Save*, as usual.

Generic transactions can save a bit of time, but if you don't feel like using
this feature, there is no need to do so; just split any complicated transactions
like this into their component parts, and enter these separately.

Recurring transactions
----------------------

Sometimes you will have a certain transaction that occurs again and again, with
the same amount, at set intervals. For example, you might get paid a 1000.00
wage every 2 weeks; or you might have 250.00 in rent deducted from you bank
account every week. It would be tedious to have to remember to enter these
transactions manually every time they occur. To avoid this tedium,
DailyCashManager supports recurring transactions. You only have to enter the
transaction once, as a recurring transaction, and then DailyCashManager will
automatically record the transaction for you at the appropriate intervals
thereafter.

To create a recurring transaction, begin by entering the transaction as you
normally would. For example, for the 250.00 rent transaction, you would select
the *Spend* transaction type, enter "250" in the amount box, select your
bank account from the *Account* box, and select "Rent" from the *Envelope*
box.

Before hitting *Save*, however, you need to select the recurring
transaction interval from the large box near the bottom left of the
transaction panel (the one that reads "Record once only, on"). Select the
required interval from the drop down list. In this case, you would select
"Record every week, starting".

Finally, in the date box near the bottom right, enter the date when the
recurring transaction will *next* occur. Hit *Save* as usual to save
the transaction. You will be prompted to enter a unique name for the
recurring transaction. You might simply enter "Rent payment" here; it's up
to you what you enter here - it's just so you can easily identify the
transaction later on. Click *OK* to confirm. You will notice that your new
recurring transaction is now recorded in the *Recurring transaction* list at the
bottom right of the main window, with the date you entered recorded in the
*Next date* column.

The first time you open your DCM file on or after the *Next date* for this
transaction, DailyCashManager will automatically record this transaction
for you; a message box will appear at that time, informing you that the
transaction has been recorded. The transaction will continue to be recorded
automatically at the intervals you specify, until you either delete it (or
edit it to record at some other interval).

Setting a recurring transaction to record on the last day of the month
......................................................................

To avoid confusion, DailyCashManager has two different types of "monthly"
frequencies. Normally if you want a transaction to be recorded every month (or
every 2 months or etc.), you would select "Record every month, starting" (or
"Record every 2 months, starting", or etc.) from the frequency selection list.
However, suppose you want to record a transaction on the *last* day of every
month; and suppose the next date that you want the transaction to be recorded
on is 30 April 2014. If you were simply to specify that you wanted this
transaction to be recorded every month, this would entail that you want it
to be recorded on the *30th* of each month; but that's not want you want at
all; because the last day of May, for example, is the 31st, not the 30th. To
avoid this confusion, DailyCashManager prevents you from entering a simple
"every month" transaction with the 29th, 30th or 31st as the next recording
date. If you want a tranaction to be recorded on the last day of each month
(which might be the 30th, 31st, 28th or 29th, depending on the month in
question), select "Record every month, on the last day of the month, starting"
from the frequency box, and this will have the desired effect.

Viewing or editing a transaction
--------------------------------

You can view or edit a transaction after you have already saved it.

First you need to select the transaction you want to view or edit:

- To select a one-off transaction for editing, click the *Transactions* tab near
  the top left of the main window (between the *Balances* and *Reconciliations*
  tabs). From the *Account or envelope* box, select one of the accounts or
  envelopes you know was involved in the transaction you are interested in. Then
  enter a date range in the *From* and *To* boxes (you can leave these blank to
  view all transactions regardless of date). Then click *Run*. A list of
  transactions, involving the selected account or envelope, will be displayed in
  the large panel below. Locate the transaction you want to edit in this list.
  You can view this transaction either by double-clicking it, or else by
  selecting it and then choosing *Edit selected ordinary transaction* from the
  *Edit* menu. This will cause the full detail of the selected transaction to
  appear in the transaction panel to the right.
- To view or edit a recurring transaction, locate it in the *Recurring
  transaction* list at the bottom right of the main window. Either double-click
  it, or else select it and then choose *Edit selected recurring transaction*
  from the *Edit* menu. This will cause the full detail of the selected
  transaction to appear in the transaction panel above.

Once the desired transaction is selected and appears in the transaction
panel, you can edit if desired. Simply change the contents of the date,
amount, *Account*, *Memo* or other boxes as desired; then click *Save* to save
your changes.

To preserve the integrity of your DCM file, there are certain aspects of the
transaction you won't be able edit; for example, you can't change a *Spend*
transaction to an *Earn* transaction. Also, if some or all lines in the
transaction have been reconciled (see `Reconciliations`_), those lines, as well
as some other aspects of the transaction such as the date, will be greyed out
and impossible to edit. This is to preserve the integrity of the reconciliation.
(Don't worry if you don't know what this means right now.) Most aspects of the
transaction can be edited however.

If you decide to abandon your changes and revert to the earlier saved version
of the transaction, simply click *Cancel* (near the bottom left of the
transaction panel, above the *Delete transaction* button). This will cause
the transaction panel to return to its default blank state, ready for a new
transaction to be entered; the transaction you had selected for editing will
still be there in your DCM file, unchanged, in the same state it was in when
you selected it; but it will no longer be showing in the transaction panel.

Note that if you edit a recurring transaction, it won't cause any changes to
instances of that recurring transaction that have already been posted; it will
change only the behaviour of that recurring transaction from that point on.

Deleting a transaction
----------------------

To delete a transaction entirely, whether a one-off transaction or a recurring
transaction, first select the transaction for editing as explained
`above`_. Then, click the *Delete transaction* at the bottom right of the
transaction panel. A message box will appear asking if you are sure you want
to delete the entire transaction. Click *Yes* to confirm deletion (or *No* to
abort the deletion and be returned to the transaction panel). Once the
transaction is deleted, account and envelope balances will be updated
accordingly: the balances will be as if the deleted transaction had
never occurred.

Split transactions
------------------

Sometimes you will have a transaction in which a given side of the transaction
involves multiple accounts or envelopes. For example, you might spend 100.00 at
the department, but perhaps 10.00 of the 100.00 is spent on "Food", and the
other 90.00 is spent on "Clothes". You can reflect this using the split
transaction feauture. Click the *Split* button, at the right of the row in the
transaction that you want to split, and it will split into two rows. You can
then split it into even more rows by clicking *Split* again; or you can remove a
row if you change your mind, by clicking *Unsplit*. If required, both side of
the transaction can be split. For example, you might pay for 40.00 of the 100.00
you spend at the department store using cash, and the remaining 60.00 using your
credit card.

When a given side of the transaction is split, a separate amount box becomes
available for each side of the transaction. The amounts entered in all the
rows for that side need to sum to the same amount as the total transaction
amount entered at the top of the transaction panel. You can enter the amount
in all the amount boxes manually; however, we see below, under `Handy
shortcuts`_, that there are often faster ways to populate the amount boxes,
than entering the number manually.

Handy shortcuts
---------------

Entering transactions is the action you will find yourself performing most
frequently when using DailyCashManager. There are some shortcuts you can use to
make the process of recording transactions quicker and easier.

Automatic balancing of split transactions
.........................................

Suppose you have entered "100.52" in the main amount box of the transaction
panel, for some *Expense* transaction. You then split the *Envelope* row into
two, and select "Food" for one row and "Household supplies" for the other row.
Say you spent 80.53 on food, and the remainder, 19.99, on household supplies.
You enter "80.53" in the "Food" row. Now, you could enter "19.99" manually in
the "Household supplies" row. But calculating this number may involve adding
several items from your receipt, or else performing a calculation
(100.52 - 80.53 = 19.99) to work out how much you spent on household supplies.
To avoid the bother of performing a calculation, you can simply double-click
in the amount box of the "Household supplies" row, and DailyCashManager will
calculate the 19.99 for you, and populate the cell with this figure.

In general, whenever you have a split transaction, and you want to populate
one of the amount cells with whatever is the "remaining" amount of the
total transaction amount that has not yet been entered, simply double-click
on the cell you want to populate, and the remaining amount will be calculated
and inserted into the cell automatically.

Performing simple calculations within an amount cell
....................................................

DailyCashManager can act as a primitive calculator. Suppose you spent
100.52 at the supermarket, but your receipt includes many items, some of
which fall under "Food", and others of which fall under "Household supplies".
If you wanted to, you could create a split transaction with a separate row
for every single item on the receipt. But this would be a bit laborious.
Instead, you decide to enter all the "Food" items in one row, and all the
"Household supplies" items in another row. But to do this you need to add up
the cost of all the food items. Instead of getting out a calculator, you
can add the cost of the food items directly, within the amount box for the
"Food" row. Suppose there are three food items, costing 1.52, 19.89 and 5.03.
Simply type "1.52+19.89+5.03" into the amount cell for this row, then hit
tab or click outside the cell. Then the result, "26.44", will automatically
appear in the cell. You can also perform subtractions, e.g. by typing
"80.89-38.60" in the cell. Note, however, that you can't do multiplication
or division, and you can't use parentheses in calculations.

Entering dates
..............

If you only enter a single number in the date box, e.g. "23", this will be
interpreted as meaning "the 23rd of the current month". Similarly, if you enter
the date and month, but omit the year, this will be interpreted as if you had
simply entered the current year.

Selecting accounts and envelopes from the *Account* and *Envelope* lists
........................................................................

If you select one account *and* one envelope in the *Account* and *Envelope*
lists (the large ones near the left and middle of the main window,
respectively), and then click *Clear* in the transaction panel, the
transaction panel will be partially populated with a transaction that is
appropriate for the select account and envelope. For example, if you
click once on "Cash" in the *Account* list, and then click once on "Food"
in the *Envelope* list, and then click *Clear*, then the transaction type
control in the transaction panel will be automatically set to *Spend*, the
*Account* box will be set to "Cash" and the *Envelope* box will be set to
"Food". This saves you having to select (or type) the name of the account and
the envelope from the drop-down lists in the transaction panel. This can save
some effort when quickly entering a simple transaction; but be aware that
clicking the *Clear* button causes whatever details are currently entered in
the transaction panel to be lost. (Clicking *Clear* doesn't actually delete
any already-saved transaction, but simply clears out the contents of the
transaction panel, ready for a new transaction to be entered.)

.. references
.. _`below`: Transactions.html#recurring-transactions
.. _`above`: Transactions.html#viewing-or-editing-a-transaction
.. _`Reconciliations`: Reconciliations.html
