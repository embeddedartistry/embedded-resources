# Contributing

Write something nice and instructive as an intro. Talk about what kind of contributions you are interested in.

> Welcome! We love receiving contributions from our community, so thanks for stopping by! There are many ways to contribute, including submitting bug reports, improving documentation, submitting feature requests, reviewing new submissions, or contributing code that can be incorporated into the project.

This document describes our development process. Following these guidelines shows that you respect the time and effort of the developers managing this project. In return, you will be shown respect in addressing your issue, reviewing your changes, and incorporating your contributions.

If you're not looking for some kinds of contributions, note that up front:

> Please, don't use the issue tracker for support questions. Instead use: a, b, or Stack Overflow.

**Table of Contents:**

1. [Code of Conduct](#code-of-conduct)
2. [Important Resources](#important-resources)
2. [Questions](#questions)
3. [Feature Requests](#feature-requests)
4. [Improving Documentation](#improving-documentation)
5. [Reporting Bugs](#reporting-bugs)
6. [Contributing Code](#contributing-code)
	1. [Getting Started](#getting-started)
	1. [Finding an Issue!](#finding-an-issue)
	1. [Development Process](#development-process)
	1. [Building the Project](#building-the-project)
	1. [Testing](#testing)
	1. [Style Guidelines](#style-guidelines)
	1. [Code Formatting Rules](#code-formatting)
	1. [Whitespace Cleanup](#whitespace-cleanup)
1. [Pull Request Guidelines](#pull-request-guidelines)
	1. [Review Process](#review-process)
	1. [Addressing Feedback](#addressing-feedback)
1. [Community](#community)


## Code of Conduct

If you have a code of conduct document:

> By participating in this project, you agree to abide by our [Code of Conduct][0]. We expect all contributors to follow the [Code of Conduct][0] and to treat fellow humans with respect.

If not, lay some ground rules down up front.

## Important Resources

Include Short Links to Important Resources:

* docs: handbook / roadmap
* bugs: issue tracker / bug report tool
* comms: forum link, developer list, IRC/email
* wiki

## Questions

How do you prefer people to ask questions? Via an issue? Is there an IRC channel, google group, or other way to get help? Should issues that are questions get a specific label? Can they email you directly?

## Feature Requests

Provide information on the process for requsting new features. Do you have a specific label that should be applied? Is sign-off needed?

If you have a roadmap, goals, works in progress, or a development philosophy, make sure to share the information. Try to make sure that users have enough information to evaluate whether a feature request is appropriate for your project up front. Examples:

> Please create a new GitHub issue for any major changes and enhancements that you wish to make. Please provide the feature you would like to see, why you need it, and how it will work. Discuss your ideas transparently and get community feedback before proceeding.

> Major Changes that you wish to contribute to the project should be discussed first in an GitHub issue that clearly outlines the changes and benefits of the feature.

> Small Changes can directly be crafted and submitted to the GitHub Repository as a Pull Request. See the section about Pull Request Submission Guidelines, and for detailed information the core development documentation.

## Reporting Bugs

**If you find a security vulnerability, do NOT open an issue. Email EMAIL@DOMAIN.COM instead.**

Ask contributors to check before filing a new issue. Also, provide any references to FAQs or debugging guides that you might have.

> Before you submit your issue, please [search the issue archive][6] - maybe your question or issue has already been identified or addressed.

Tell contributors how to file a useful bug report. What information do you need? (e.g. version, architecture, log files, expected behavior, observed behavior).

> If you find a bug in the source code, you can help us by [submitting an issue to our GitHub issue tracker][6]. Even better, you can submit a Pull Request with a fix.

Does your project use an issue template? Provide instructions and expectations for filling it out.

Have you seen an awesome issue report? Link to it for others to view!

## Improving Documentation

Include notes for how users can improve the documentation.

> Should you have a suggestion for the documentation, you can open an issue and outline the problem or improvement you have - however, creating the doc fix yourself is much better!

> If you want to help improve the docs, it's a good idea to let others know what you're working on to minimize duplication of effort. Create a new issue (or comment on a related existing one) to let others know what you're working on. If you're making a small change (typo, phrasing) don't worry about filing an issue first.

> For large fixes, please build and test the documentation before submitting the PR to be sure you haven't accidentally introduced any layout or formatting issues.

```
Provide instructions on buildling and viewing documentation
```

## Contributing Code

This section is used to get new contributors up and running with dependencies, development, testing, style rules, formatting rules, and other things they should know.

If you have a label for beginner issues, talk about that here so they know where to look:

> Unsure where to begin contributing to Atom? You can start by looking through these beginner and help-wanted issues: Beginner issues - issues which should only require a few lines of code, and a test or two. Help wanted issues - issues which should be a bit more involved than beginner issues.

Working on your first open source project or pull request? Her are some helpful tutorials:

* [How to Contribute to an Open Source Project on GitHub][2]
* [Make a Pull Request][3]
* [First Timers Only][4]

### Getting Started

Install these dependencies:

```
with some examples
```

Provide some instructions for your workflow (e.g. fork the repository)

> You will need to fork the main repository to work on your changes. Simply navigate to our GitHub page and click the "Fork" button at the top. Once you've forked the repository, you can clone your new repository and start making edits.

> In git it is best to isolate each topic or feature into a “topic branch”. While individual commits allow you control over how small individual changes are made to the code, branches are a great way to group a set of commits all related to one feature together, or to isolate different efforts when you might be working on multiple topics at the same time.

> While it takes some experience to get the right feel about how to break up commits, a topic branch should be limited in scope to a single issue

```
# Checkout the master branch - you want your new branch to come from master
git checkout master

# Create a new branch named newfeature (give your branch its own simple informative name)
git branch newfeature

# Switch to your new branch
git checkout newfeature
```

For more information on the GitHub fork and pull-request processes, [please see this helpful guide][5].

### Finding an Issue

The list of outstanding feature requests and bugs can be found on our on our [GitHub issue tracker][6]. Pick an unassigned issue that you think you can accomplish and add a comment that you are attempting to do it.

Provide notes on different kinds of issues or labels

> `starter` labeled issues are deemed to be good low-hanging fruit for newcomers to the project
> `help-wanted` labeled issues may be more difficult than `starter` and may include new feature development
> `doc` labeled issues must only touch content in the `docs` folder.

### Development Process

What is your development process?

> This project follows the [git flow](http://nvie.com/posts/a-successful-git-branching-model/) branching model of product development.

Talk about branches people should work on. Sspecifically, where is the starting point? `master`, `development`, etc.

> You should be using the master branch for the most stable release; please review [release notes](https://github.com/openopps/openopps-platform/releases) regularly. We do releases every week or two and send out notes. If you want to keep up with the latest changes, we work in the `dev` branch.  If you are using dev, keep an eagle-eye on commits and/or join our daily standup.

### Building the Project

What branches should be work be started off of?

Include instructions on how to build the project.

```
with some examples
```

Provide instructions on adding a new file/module to the build

```
with some examples
```

Keep your tests as simple as possible.

### Testing

If you add code you need to add tests! We’ve learned the hard way that code without tests is undependable. If your pull request reduces our test coverage because it lacks tests then it will be rejected.

Provide instructions for adding new tests. Provide instructions for running tests.

```
with examples
```

### Style Guidelines

If your code has any style guidelines, add them here or provide links to relevant documents. If you have an automated checker, make sure to provide instructions on how to run it.

### Code Formatting

If your code has any formatting guidelines that aren't covered in the style guidelines above, add them here.

If you're using an auto-formatting tool like clang-format

```
Provide instructions for running the formatting tool
```

### Whitespace Cleanup

Don’t mix code changes with whitespace cleanup! If you are fixing whitespace, include those changes separately from your code changes. If your request is unreadable due to whitespace changes, it will be rejected.

> Please submit whitespace cleanups in a separate pull request.

### Git Commit Guidelines

Do you have any guidelines for your commit messages? Include them here.

> The first line of the commit log must be treated as as an email
subject line.  It must be strictly no greater than 50 characters long.
The subject must stand on its own and not only make external
references such as to relevant bug numbers.

> The second line must be blank.

> The third line begins the body of the commit message (one or more
paragraphs) describing the details of the commit.  Paragraphs are each
separated by a blank line.  Paragraphs must be word wrapped to be no
longer than 76 characters.

> The last part of the commit log should contain all "external
references", such as which issues were fixed.

> For further notes about git commit messages, [please read this blog post][7].

## Pull Request Process

Do you have any labelling conventions?

Add notes for pushing your branch:

> When you are ready to generate a pull request, either for preliminary review, or for consideration of merging into the project you must first push your local topic branch back up to GitHub:

```
git push origin newfeature
```

Include a note about submitting the PR:

> Once you've committed and pushed all of your changes to GitHub, go to the page for your fork on GitHub, select your development branch, and click the pull request button. If you need to make any adjustments to your pull request, just push the updates to your branch. Your pull request will automatically track the changes on your development branch and update.

1. Ensure any install or build dependencies are removed before the end of the layer when doing a
   build.
2. Update the README.md with details of changes to the interface, this includes new environment
   variables, exposed ports, useful file locations and container parameters.
3. Increase the version numbers in any examples files and the README.md to the new version that this
   Pull Request would represent. The versioning scheme we use is [SemVer](http://semver.org/).
4. You may merge the Pull Request in once you have the sign-off of two other developers, or if you
   do not have permission to do that, you may request the second reviewer to merge it for you.

### Review Process

Who reviews it? Who needs to sign off before it’s accepted? When should a contributor expect to hear from you? How can contributors get commit access, if at all?

> The core team looks at Pull Requests on a regular basis in a weekly triage meeting that we hold in a public Google Hangout. The hangout is announced in the weekly status updates that are sent to the puppet-dev list. Notes are posted to the Puppet Community community-triage repo and include a link to a YouTube recording of the hangout. After feedback has been given we expect responses within two weeks. After two weeks we may close the pull request if it isn't showing any activity.

> Except for critical, urgent or very small fixes, we try to leave pull requests open for most of the day or overnight if something comes in late in the day, so that multiple people have the chance to review/comment.  Anyone who reviews a pull request should leave a note to let others know that someone has looked at it.  For larger commits, we like to have a +1 from someone else on the core team and/or from other contributor(s).  Please note if you reviewed the code or tested locally -- a +1 by itself will typically be interpreted as your thinking its a good idea, but not having reviewed in detail.

Perhaps also provide the steps your team will use for checking a PR. Or discuss the steps run on your CI server if you have one. This will help developers understand how to investigate any failures or test the process on their own.

### Addressing Feedback

Once a PR has been submitted, your changes will be reviewed and constructive feedback may be provided. Feedback isn't meant as an attack, but to help make sure the highest-quality code makes it into our project. Changes will be approved once required feedback has been addressed.

If a maintainer asks you to "rebase" your PR, they're saying that a lot of code has changed, and that you need to update your fork so it's easier to merge.

To update your forked repository, follow these steps:

```
# Fetch upstream master and merge with your repo's master branch
git fetch upstream
git checkout master
git merge upstream/master

# If there were any new commits, rebase your development branch
git checkout newfeature
git rebase master
```

If too much code has changed for git to automatically apply your branches changes to the new master, you will need to manually resolve the merge conflicts yourself.

Once your new branch has no conflicts and works correctly, you can override your old branch using this command:

```
git push -f
```

Note that this will overwrite the old branch on the server, so make sure you are happy with your changes first!

## Community

Do you have a mailing list, google group, slack channel, IRC channel? Link to them here.

> Anyone actively contributing to or using OpenOpps should join our [Mailing List](https://groups.google.com/forum/#!forum/openopps-platform).
We also have a public Slack chat team. If you're interested in following along with the development process or have questions, feel free to [join us](https://openopps.slack.com).

Include Other Notes on how people can contribute

* You can help us answer questions our users have here:
* You can help build and design our website here:
* You can help write blog posts about the project by:
* You can help with newsletters and internal communications by:

* Create an example of the project in real world by building something or
showing what others have built.
* Write about other people’s projects based on this project. Show how
it’s used in daily life. Take screenshots and make videos!

[0]: docs/CODE_OF_CONDUCT.md
[1]: style_guidelines.md
[2]: https://egghead.io/series/how-to-contribute-to-an-open-source-project-on-github
[3]: http://makeapullrequest.com/
[4]: http://www.firsttimersonly.com
[5]: https://gist.github.com/Chaser324/ce0505fbed06b947d962
[6]: link/to/your/project/issue/tracker
[7]: http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html
