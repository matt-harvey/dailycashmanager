Budgeting
=========

If you want to, you can use DailyCashManager simply as a means to record
your financial transactions as they occur, keeping track of your asset and
liability balances, your earnings, and the amounts you spend on different
categories of expenditure. If you use DailyCashManager in this way, you will
build up a record of *historical* financial data. It can be quite useful to
have this record; but it will be purely "backward-looking". To really
manage your finances effectively, you'll want to know not only how much
money you *have* spent on one category or another, but also, how much you
*should* spend on one category or another, if you want to meet your
financial goals. That is, you'll want to use DailyCashManager not only as a
historical record, but also as a planning mechanism, and a mechanism for taking
*control* of your spending so that you can reach your financial goals.

Envelope budgeting
------------------

To facilitate this planning and control function, DailyCashManager
implements a system known as *envelope budgeting*. Envelope budgeting is a
well-tried financial strategy that involves allocating your money, as you earn
it, into different "envelopes", to save for different categories of expenditure.
Traditional envelope budgeting would involve physically placing cash into
physical envelopes out of your "paycheck" each time you were paid. If you knew
you spent on average 1000.00 a year, say, on electricity bills, and if you were
paid monthly, you would have put 83.33 per month into the "Electricity"
envelope. Then when the electicity bill arrived, there would be a bunch of money
sitting in the electricity envelope, set aside just for the purpose of paying
the electricity bill. You would remove the amount of the bill from the
electricity envelope, and pay the bill.

These days, most people don't get paid in physical cash; we keep most of
our money in bank accounts, shares, term deposits or other assets; and many
people have credit cards, loans and other liabilities offsetting those assets.
These accounts are digital in nature; and so the crude method of dividing
physical cash into different envelopes is pretty awkward to apply. That's where
an application like DailyCashManager comes in.

Instead of physical envelopes, DailyCashManager facilitates the creation of
virtual envelopes, each envelope representing a different category of revenue
or expenditure. And instead of allocating a blob of cash into each envelope
with each paycheck, DailyCashManager allocates a small, steady amount of
money to each envelope, on a *daily* basis.

Let's say you receive 1000.00 cash, paid into your "XYZ Bank saving" bank
account. You would enter a revenue transaction in DailyCashManager, with "XYZ
Bank saving" as the account, and "Salary" as the envelope. The balance of "XYZ
Bank saving" would increase by 1000.00, and the balance of "Salary" would also
increase by 1000.00.

Each day thereafter - depending on how you have set up your *budget items*
(which is explained below) - a small amount of that 1000.00 would be transferred
automatically by DailyCashManager from the "Salary" envelope, into each of your
expense envelopes, to set aside towards each category of expenditure. Let's now
see how to set up your *budget items* to make this happen.

Creating and editing budget items
---------------------------------

A *budget item* represents a given expense or revenue item for which you want
to set aside money in an envelope. Often you will just want to create a single
budget item for a given envelope; but you can assign multiple budget items to
a given envelope if you wish.

For example, you might have an expense envelope called "Bank fees". Within this
envelope, you record two different ongoing fees: a 2.00 monthly account fee; a
50.45 annual credit card fee. You would create one budget item for the
monthly account fee, and another budget item for the credit card fee. You would
do this as follows:

- Open the envelope editing panel for "Bank fees", either by double-clicking
  "Bank fees" in the *Envelope* list, or by selecting "Bank fees" in the
  *Envelope* list and then selecting *Edit selected envelope* form the *Edit*
  menu.
- Within the envelope editing panel, click *Add item*. A new row will appear
  under *BUDGET ITEMS*. Enter "Monthly account fee" in the *Description* box
  and "2" in the *Amount* box, and select "per month" from the
  *Frequency* box.
- Click *Add item* again. This will produce another row of boxes. Enter
  "Credit card fee" in the *Description* box and "50.45" in the *Amount* box,
  and select "per 12 months" from the *Frequency* box.
- Notice that just above the words "BUDGET ITEMS", the *Daily top up* amount
  will read "0.20". This means that, in order to set aside enough money to cover
  the two budget items you have entered, DailyCashManager will automatically
  transfer an amount of 0.20 into the "Bank account" envelope every day. This
  is calculated as follows: (2 * 12) + 50.45 / 365.25 = 0.20.
- Click *Save*. Another window will now appear informing you that the
  "Budget is now out of balance by an amount of (0.20)." This is saying that,
  in order to transfer the 0.20 into the "Bank fees" envelope each day,
  DailyCashManager needs transfer that 0.20 *from* some other envelope.
  By default - and if you click *Leave unbalanced* this deafult will be
  unchanged - the 0.20 is transferred from the envelope called "Budget
  imbalance". This is an envelope which DailyCashManager creates automatically
  to process unbalanced budget amounts. You *don't* normally want "Budget
  imbalance" to have a balance other than *zero*. Instead of
  clicking "Leave unbalanced", you should select some other envelope to draw
  the 0.20 / day from - by selecting from the drop-down list at the bottom
  of this panel - and then click *Offset to envelope below*. Commonly, you
  would offset the 0.20 / day to your "Salary" envelope (or whatever is your
  main source of ongoing income, e.g. "Pension", "Allowance" or etc..). This
  means that every day, 0.20 will be transferred from your "Salary"
  envelope to your "Bank fees" envelope, to provide for both your monthly
  account fee and your annual credit card fee.
- After clicking *Save*, the envelope editing panel will disappear. Notice
  that in the *Envelope* list near the centre of the main window, there will
  be a "0.20" in the *Daily top-up* column, next to "Bank fees".

Using envelopes to make sure you are living within your means
-------------------------------------------------------------

In DailyCashManager, the amount in an expense envelope at a given point in time
represents the amount that is available to spend on that category of expenditure
at that time. Budgeting using the DailyCashManager envelope system means
striving not to spend any more on each expenditure category than what is
available in the corresponding envelope at a given point in time. You can think
an envelope in DCM as just the electronic equivalent of physical cash in a
physical envelope: if the money isn't there to spend, you can't (or rather
shouldn't) spend it.

Sometimes, of course, reality asserts itself, and you may have no realistic
choice but to spend more than what's in a given envelope for a given
expenditure category. If this happens, you should still enter the transaction as
usual; and this will cause the envelope balance to become negative.

Responding to a negative envelope balance
-----------------------------------------

If the balance in an envelope becomes negative, it usually makes
sense, first of all, to examine the budget items for that envelope, and
realistically ask yourself whether they are set at adequate levels. There
may simply not be enough allocated to some budget item; or perhaps you should
create an additional budget item to cover off some item of expenditure which
you hadn't considered previously. To get some idea of whether your ongoing
budget item allocations are realistic, you might want to run an
`income and expenditure report`_ and look at the average daily expenditure
for the relevant envelope for, say, the last 6 or 12 months (or whatever time
period you have available), and compare this to the daily allocation for that
envelope that is being generated by your budget items. (Depending on the time
person, you might want to allow for inflation when making this comparison.)

Often, if the envelope hasn't gone *too* far into a negative balance, the
best response is simply to wait: continue on, trying if possible not
to spend any more from the envelope with the negative balance until it has
become positive, and then restricting one's expenditure on that category
to whatever positive balance is available in that envelope thereafter.

If there is a *large* negative balance, however, you might want to look
for another envelope in which you have excess funds - funds which you don't
realistically think you will require for that other category of expenditure -
and transfer some of those excess funds into the negative envelope, if possible
restoring the latter to a positive balance. See
`Transferring money between envelopes`_.

Transferring money between envelopes
------------------------------------

Sometimes you find that one envelope contains more than excess funds, while
another contains a shortfall. If this occurs, you should review the budget
items in each envelope to see whether the ongoing daily allocations to each
are adequate, or inadequate, and edit these as required. However, as a one-off
measure, you may want to reallocate funds from one envelope to another. This
should be done sparingly - after all, if you always shift money around to
wherever it is required to wherever you want it, you will defeat the purpose
of budgeting entirely - but sometimes it does make sense to do this.

To shift money between envelopes, select *New envelope transfer* from the
*New* menu at the top of the main window. Enter the amount you want to transfer;
select the envelope to transfer *from* in the *Source* box; and select the
envelope you want to transfer the funds into, from the *Destination* box.
Then click *OK* to proceed with the transfer, or *Cancel* to abort.

On returning to the main window, you will notice that the balances of the
affected envelopes have been updated accordingly.

Targetted saving using pure envelopes
-------------------------------------

Usually when you create an envelope_, it will be either a *revenue* or an
*expense* envelope. However, DailyCashManager provides for a third kind of
envelope, called a *pure envelope*. A pure envelope is one from which it is
not possible to record any *actual* transactions. Rather, you use it to
accumulate funds towards some special goal or other, and then when the
money is required to be spent on that goal, you transfer it out of the
pure envelope, and into an ordinary expense envelope, and spend it from the
expense envelope.

For example, you might know that, on average, you spend 100.00 on food during
the week leading up to Christmas, over and above your usual weekly spending
on food. Accordingly, you could create a pure envelope called "Christmas food
fund". You would create this in the same way as you would create any other
envelope (see `Creating an envelope`_), except that in the envelope creation
panel, instead of selecting *revenue* or *expense* from the *envelope type* box,
you select *pure envelope*. You would then create a `budget item`_ under this
envelope, for allocating 100.00 per twelve months towards Christmas food
expenditure. Then, when the week leading up to Christmas comes round (assuming
you created this budget item one year before), you would transfer 100.00 out of
the "Christmas food fund" pure envelope, and into your usual "Food" expense
envelope, and spend it out of "Food".

.. References
.. _envelope: Envelopes.html
.. _`Creating an envelope`: Envelopes.html#creating-an-envelope
.. _`budget item`: Budgeting.html#creating-and-editing-budget-items
.. _`income and expenditure report`: Reports.html#income-and-expenditure
