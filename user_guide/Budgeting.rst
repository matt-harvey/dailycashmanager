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
you spent on average $1000 a year, say, on electricity bills, and if you were
paid monthly, you would have put $83.33 a month into the "Electricity" envelope.
Then when the electicity bill arrived, there would be a bunch of money sitting
in the electricity envelope, set aside just for the purpose of paying the
electricity bill. You would remove the amount of the bill from the electricity
envelope, and pay the bill.

These days, most people don't get paid in physical cash; we keep most of
their money in bank accounts, shares, term deposits or other assets; and many
people have credit cards, loans and other assets offsetting those assets. These
accounts are digital in nature; and so the crude method of dividing physical
cash into different envelopes is pretty awkward to apply. That's where an
application like DailyCashManager comes in.

Instead of physical envelopes, DailyCashManager facilitates the creation of
virtual envelopes, each envelope representing a different category of revenue
or expenditure. And instead of allocating a blob of cash into each envelope
with each paycheck, DailyCashManager allocates a small, steady amount of
money to each envelope, on a *daily* basis.

Let's say you receive $1000 cash, paid into your "XYZ Bank saving" bank account.
You would enter a revenue transaction in DailyCashManager, with "XYZ Bank
saving" as the account, and "Salary" as the envelope. The balance of "XYZ Bank
saving" would increase by $1000, and the balance of "Salary" would also increase
by $1000.

Each day thereafter - depending on how you have set up your *budget items*
(which is explained below) - a small amount of that $1000 would be transferred
automatically by DailyCashManager from the "Salary" envelope, into each of your
expense envelopes, to set aside towards each category of expenditure. Let's now
see how to set up your *budget items* to make this happen.

Creating a budget item
----------------------

Editing a budget item
---------------------

Targetted saving using pure envelopes
-------------------------------------
