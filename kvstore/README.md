Project 5A - Concurrent Store
====================

<!-- TODO: Fill this out. -->

## Design Overview:
None
## Collaborators:
None
## Extra Credit Attempted:
None
## How long did it take to complete the Concurrent Store portion of KVStore?
25 hours
<!-- Enter an approximate number of hours that you spent actively working on the project. -->


Project 5B - Distributed Store
====================

<!-- TODO: Fill this out. -->

## Design Overview:
1. Who was your stakeholder pair? (<1 sentence)

Pair #5: Matt Bleat vs. The Center for Changing Our Campus Culture

2. What kind of delete did you implement and why? Explain your decisions as if you were reporting to an ethical auditor who will determine whether your design is justified. Highlight and explain what you think is the most compelling reason that supports the specific kind of deletion you’ve implemented (1-2 short paragraphs)

My deletion implemention will remove all of the user's personal posts, and remove the affiliation between the user's personal posts and any replies to those posts. The deletion will also remove all tags of @user_id in other users' posts and comments, as well as any mentions of the user's username. This includes hashtags that includes the username, as they can be used to directly find the user's account in question (we are not actually deleting the user's account, so it is still searchable by anyone on the platform). However, the posts and replies made by other users will remain, as deleting these posts would infringe on the other users' freedom of speech/expression.

This implementation aligns with Article 17 GDPR's Right to Erasure, because it gives data subjects "the right to obtain from the controller the erasure of personal data concerning him or her," where we define "personal data" as any text containing the user's username or a tag of the user. This effectively removes any surface-level connection on Tweeter between the the controversial topics and the user, Matt. However, the controversial posts and replies related to the user will remain, along with any other hashtags or tags. This serves as a balance with the opposing stakeholder, The Center for Changing Our Campus Culture. Posts will still contain tags to @brownusports, so the activism and attempts to draw attention to Brown's institution will still be there on the platform. Additionally, it would not be hard for the University and those involved in the situation to make the connection between Matt and what the posts/replies are referring to, so it is perfectly possible for his offer of admission to be rescinded still. So, the deletion essentially anonymizes Matt's identity on Tweeter and prevents others from discussing him in a targeted, hurtful way while still allowing the situation to be talked about extensively and for the Center for Changing Our Campus Culture's missions of public good and Title IX awareness to be fulfilled.

3. What are the shortcomings of your implementation? Who are some short term and/or long term stakeholders (beyond the ones we’ve asked you to consider) who could be adversely affected by your decision? (1-2 short paragraphs)

One shortcoming of this implementation is since hashtags aren't tied down to one user or idea, it is possible that by censoring a hashtag that contains Matt's name, we are inadvertently censoring a completely different use for the hashtag that is unrelated to this situation. Additional natural language processing would need to be implemented to determine the intent of the post that's using the hashtag, so that only those related to the specified user would be deleted. 

Additionally, the people who are tangentially involved in the situation, such as Matt's family, classmates, teammates, the victims in the sexual harassment allegations, or Brown University could face the brunt of the backlash online in place of Matt. And now that Matt's connection is removed from the narrative, the negativity previously associated with Matt's actions could now be fully associated with Brown, which could lead to misconceptions and misleading narrative online that could tarnish the reputation of the University.


4. How might your approach to this assignment change if you were asked to consider the interests of other stakeholders not mentioned in the scenario? (1-2 short paragraphs)

I would first consider the stakeholder position of Brown University, who have already ruled on the situation and concluded that there was insufficient evidence to support those allegations and declined to take further action. The position of the University in this situation would likely be to reduce Matt's and their association with the situation as much as possible. However, since it is only Matt that is invoking the GDPR Right to Erasure, there is minimal additional actions that can be taken aside from completely removing the posts, which would go against the other user's freedom of speech/expression. 

Another main stakeholder with be the victims in the sexual harassment allegations, who depending on their position might take different sides in the conflict. They could decide to want nothing to do with this, which would require our deletion algorithm to take into account the privacy of additional people, which is not feasible due to same reason as above. They could also take the side of the Center for Changing Our Campus Culture and want the situation to be greater publicized in the hopes of taking further legal action in the sexual harassment case. In this case, the erasure would no longer be able to go through as it is being used "for the establishment, exercise or defence of legal claims." In this case, we would have to honor any legally binding contract or warrant issued by the court and maintain any court-admissable evidence on Tweeter. This would mean keeping all of the posts related to Matt's situation exactly as they were originally, and not censoring hashtags or comments.

## Collaborators:
None

## Extra Credit Attempted:
None

## How long did it take to complete the Distributed Store portion of KVStore?

<!-- Enter an approximate number of hours that you spent actively working on the project. -->
20